/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * jc 코루틴(stackful, MASM) vs C++20 코루틴(stackless) vs Windows Fiber 성능 비교
 *
 * 비교 축
 *   1. Lifecycle  : 생성 → 즉시 종료 → 해제 비용 (jc는 풀 재사용 경로 = decommit/commit 비용 포함)
 *   2. Switch     : 단일 코루틴 yield/resume 왕복 1회 비용 (컨텍스트 스위치 순수 비용)
 *   3. RoundRobin : M개 코루틴을 라운드로빈으로 1회씩 resume (스케줄러 시나리오, 캐시/TLB 영향)
 *   4. DeepYield  : 깊이 D 콜스택 끝에서 yield (stackful이 유리한 시나리오,
 *                   stackless는 D개 프레임 할당 + symmetric transfer 체인 필요)
 *   5. StackGrowth: 큰 지역 배열로 스택 확장 유발 (jc VEH 확장 vs jc 전량 선커밋 vs Fiber 커널 확장)
 *
 * 읽는 법
 *   - Time 컬럼 = iteration 1회의 ns. Switch 계열은 iteration 1회 = 왕복 1회이므로 그대로 ns/왕복.
 *   - items_per_second 가 있는 벤치는 1e9 / items_per_second = ns/item.
 *
 * 주의
 *   - jc 코루틴은 CoVEH가 등록돼 있어야 스택 자동 확장이 동작한다. (EnsureCoVEH)
 *   - Fiber 는 fiber proc 에서 return 하면 스레드가 종료되므로 반드시 SwitchToFiber(main)으로 복귀시킨다.
 *   - Release x64 로 측정할 것. Debug 는 jc_assert / dbg_new 오버헤드가 섞인다.
 */

#include "jc/Threading/Coroutine.h"

#if CoroutineTest == ON

#pragma warning(push, 0)
#include <coroutine>
#include <exception>
#include <vector>
#include <utility>
#pragma warning(pop)

// ========================================================================================
// 공통
// ========================================================================================

static void EnsureCoVEH()
{
	static void* s_pVeh = nullptr;
	if (s_pVeh == nullptr)
		s_pVeh = AddVectoredExceptionHandler(1, CoVEH);
}

// 벤치마크 함수 → 코루틴 함수로 값 전달용 (CoContext에 userData가 없어서 전역 사용)
static volatile bool	g_stop			= false;
static int				g_depth			= 0;
static int				g_touchBytes	= 0;
static volatile int		g_sink			= 0;

// ========================================================================================
// C++20 stackless 타입
// ========================================================================================

// [단일 레벨] co_yield 기반 제너레이터 — Switch / RoundRobin 벤치용
struct StacklessGen
{
	struct promise_type
	{
		int value_ = 0;
		StacklessGen get_return_object() { return StacklessGen{ std::coroutine_handle<promise_type>::from_promise(*this) }; }
		std::suspend_always initial_suspend() noexcept { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		std::suspend_always yield_value(int _v) noexcept { value_ = _v; return {}; }
		void return_void() noexcept {}
		void unhandled_exception() noexcept { std::terminate(); }
	};
	using handle = std::coroutine_handle<promise_type>;

	handle h_{};
	explicit StacklessGen(handle _h) : h_(_h) {}
	StacklessGen(StacklessGen&& _o) noexcept : h_(std::exchange(_o.h_, {})) {}
	StacklessGen(const StacklessGen&) = delete;
	~StacklessGen() { if (h_) h_.destroy(); }

	bool Resume() { h_.resume(); return !h_.done(); }
};

// [중첩] co_await 가능한 Task — DeepYield 벤치용
//   - 자식 Task 를 co_await 하면 symmetric transfer 로 자식으로 진입
//   - 자식이 끝나면 부모(continuation_)로 symmetric transfer
//   - leaf 에서 YieldToDriver 를 co_await 하면 root promise 에 leaf 핸들을 기록하고 전체 체인이 suspend
//   - 드라이버(벤치 루프)는 root.leaf_ 를 resume 한다  (cppcoro::recursive_generator 와 같은 접근)
struct StacklessTask
{
	struct promise_type;
	using handle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		handle			continuation_{};		// 나를 co_await 한 부모
		promise_type*	root_ = nullptr;		// 최상위 promise
		handle			leaf_{};				// root 에서만 의미 있음: 현재 suspend 된 leaf

		StacklessTask get_return_object() { return StacklessTask{ handle::from_promise(*this) }; }
		std::suspend_always initial_suspend() noexcept { return {}; }

		struct FinalAwaiter
		{
			bool await_ready() noexcept { return false; }
			std::coroutine_handle<> await_suspend(handle _h) noexcept
			{
				promise_type& p = _h.promise();
				if (p.continuation_)
				{
					p.root_->leaf_ = p.continuation_;
					return p.continuation_;			// 부모로 symmetric transfer
				}
				return std::noop_coroutine();		// root 종료 → 드라이버로 복귀
			}
			void await_resume() noexcept {}
		};
		FinalAwaiter final_suspend() noexcept { return {}; }
		void return_void() noexcept {}
		void unhandled_exception() noexcept { std::terminate(); }
	};

	handle h_{};
	explicit StacklessTask(handle _h) : h_(_h) {}
	StacklessTask(StacklessTask&& _o) noexcept : h_(std::exchange(_o.h_, {})) {}
	StacklessTask(const StacklessTask&) = delete;
	~StacklessTask() { if (h_) h_.destroy(); }

	// co_await 지원 (awaiter 역할)
	bool await_ready() noexcept { return false; }
	std::coroutine_handle<> await_suspend(handle _parent) noexcept
	{
		promise_type& p = h_.promise();
		p.continuation_	= _parent;
		p.root_			= _parent.promise().root_;
		p.root_->leaf_	= h_;
		return h_;									// 자식으로 symmetric transfer
	}
	void await_resume() noexcept {}

	// 드라이버 API
	void Start()  { h_.promise().root_ = &h_.promise(); h_.promise().leaf_ = h_; }
	bool Resume() { h_.promise().leaf_.resume(); return !h_.done(); }
};

// leaf 에서 "스케줄러로 양보"
struct YieldToDriver
{
	bool await_ready() noexcept { return false; }
	void await_suspend(StacklessTask::handle _h) noexcept { _h.promise().root_->leaf_ = _h; }
	void await_resume() noexcept {}
};

// ========================================================================================
// Windows Fiber 헬퍼
// ========================================================================================

struct FiberEnv
{
	LPVOID pMain_ = nullptr;
	bool   converted_ = false;

	FiberEnv()
	{
		if (IsThreadAFiber())
			pMain_ = GetCurrentFiber();
		else
		{
			pMain_ = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
			converted_ = true;
		}
	}
	~FiberEnv()
	{
		if (converted_)
			ConvertFiberToThread();
	}
};

struct FiberArg
{
	LPVOID pMain_ = nullptr;
};

// 즉시 main 으로 복귀 (Lifecycle)
static void CALLBACK FiberProc_Empty(void* _p)
{
	FiberArg* pArg = (FiberArg*)_p;
	for (;;) SwitchToFiber(pArg->pMain_);		// return 금지 (스레드 종료됨)
}

// g_stop 까지 무한 양보 (Switch / RoundRobin)
static void CALLBACK FiberProc_YieldForever(void* _p)
{
	FiberArg* pArg = (FiberArg*)_p;
	while (!g_stop) SwitchToFiber(pArg->pMain_);
	for (;;) SwitchToFiber(pArg->pMain_);
}

// g_touchBytes 만큼 스택 터치 후 복귀 (StackGrowth)
static void CALLBACK FiberProc_Touch(void* _p);

// ========================================================================================
// 공용 페이로드
// ========================================================================================

// 스택을 위에서 아래로 페이지 단위로 터치 (_alloca → __chkstk 경로를 타므로 TEB StackLimit 로직까지 함께 검증됨)
__declspec(noinline) static void TouchStack(int _bytes)
{
	volatile char* p = (volatile char*)_alloca(_bytes);
	for (int i = _bytes - 1; i >= 0; i -= CO_PAGE_SIZE)
		p[i] = (char)i;
	g_sink = g_sink + p[0];
}

static void CALLBACK FiberProc_Touch(void* _p)
{
	FiberArg* pArg = (FiberArg*)_p;
	for (;;)
	{
		TouchStack(g_touchBytes);
		SwitchToFiber(pArg->pMain_);
	}
}

// ========================================================================================
// jc 코루틴 함수들
// ========================================================================================

static void fn_Empty(CoContext*) {}

static void fn_YieldForever(CoContext*)
{
	while (!g_stop) CoYield();
}

static void DeepYieldRecursive(int _depth)
{
	volatile char pad[64];						// 프레임 크기를 어느 정도 확보 (stackless 프레임과 비슷한 급)
	pad[0] = (char)_depth;
	if (_depth == 0) { CoYield(); g_sink = g_sink + pad[0]; return; }
	DeepYieldRecursive(_depth - 1);
	g_sink = g_sink + pad[0];
}

static void fn_DeepYieldForever(CoContext*)
{
	while (!g_stop) DeepYieldRecursive(g_depth);
}

static void fn_Touch(CoContext*)
{
	TouchStack(g_touchBytes);
}

// ========================================================================================
// C++20 코루틴 함수들
// ========================================================================================

static StacklessGen Gen_Empty()
{
	co_return;
}

static StacklessGen Gen_YieldForever()
{
	while (!g_stop) co_yield 0;
}

static StacklessTask Task_Descend(int _depth)
{
	if (_depth == 0)
	{
		co_await YieldToDriver{};
		co_return;
	}
	co_await Task_Descend(_depth - 1);
}

static StacklessTask Task_DeepYieldForever()
{
	while (!g_stop)
		co_await Task_Descend(g_depth);
}

// ========================================================================================
// 1. Lifecycle : 생성 → 즉시 종료 → 해제
// ========================================================================================

static void BM_Lifecycle_Jc(State& state)
{
	EnsureCoVEH();
	const CoStackTier tier = (CoStackTier)state.range(0);

	for (auto _ : state)
	{
		// fn 이 yield 없이 종료 → CoFnEndTrampoline → CoFreeCtx → nullptr 반환
		// (첫 iteration 이후엔 풀 재사용 경로: PopFront + InitStack(commit) + TreeMap Insert/Pop + decommit)
		CoContext* pCtx = CoRun(fn_Empty, tier);
		DoNotOptimize(pCtx);
	}
	state.SetItemsProcessed(state.iterations());
	state.SetLabel(tier == cstLow ? "cstLow(16KB)" : tier == cstMid ? "cstMid(64KB)" : "cstHigh(256KB)");
}

static void BM_Lifecycle_Cpp20(State& state)
{
	for (auto _ : state)
	{
		StacklessGen g = Gen_Empty();		// 프레임 heap 할당
		g.Resume();							// 즉시 종료
		DoNotOptimize(g.h_);
	}										// 소멸자에서 destroy
	state.SetItemsProcessed(state.iterations());
}

static void BM_Lifecycle_Fiber(State& state)
{
	FiberEnv env;
	FiberArg arg{ env.pMain_ };
	const SIZE_T reserve = (SIZE_T)state.range(0);

	for (auto _ : state)
	{
		LPVOID pFiber = CreateFiberEx(CO_PAGE_SIZE, reserve, FIBER_FLAG_FLOAT_SWITCH, FiberProc_Empty, &arg);
		SwitchToFiber(pFiber);
		DeleteFiber(pFiber);
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_Lifecycle_Jc)->Arg(cstLow)->Arg(cstMid)->Arg(cstHigh);
BENCHMARK(BM_Lifecycle_Cpp20);
BENCHMARK(BM_Lifecycle_Fiber)->Arg(CO_STACK_SIZE_LOW)->Arg(CO_STACK_SIZE_MID)->Arg(CO_STACK_SIZE_HIGH);

// ========================================================================================
// 2. Switch : yield/resume 왕복 1회  (iteration 1회 = 왕복 1회 → Time 컬럼이 곧 ns/왕복)
// ========================================================================================

static void BM_Switch_Jc(State& state)
{
	EnsureCoVEH();
	g_stop = false;
	CoContext* pCtx = CoRun(fn_YieldForever, cstLow);	// 첫 yield 까지 실행된 상태

	for (auto _ : state)
	{
		pCtx = CoResume(pCtx);								// resume → CoYield → 복귀
	}

	g_stop = true;
	while (pCtx) pCtx = CoResume(pCtx);						// 정상 종료시켜 컨텍스트 반환
	state.SetItemsProcessed(state.iterations());
}

static void BM_Switch_Cpp20(State& state)
{
	g_stop = false;
	StacklessGen g = Gen_YieldForever();
	g.Resume();												// 첫 co_yield 까지 실행

	for (auto _ : state)
	{
		g.Resume();
	}

	g_stop = true;
	while (g.Resume()) {}
	state.SetItemsProcessed(state.iterations());
}

static void BM_Switch_Fiber(State& state)
{
	FiberEnv env;
	FiberArg arg{ env.pMain_ };
	g_stop = false;
	LPVOID pFiber = CreateFiberEx(CO_PAGE_SIZE, CO_STACK_SIZE_LOW, FIBER_FLAG_FLOAT_SWITCH, FiberProc_YieldForever, &arg);

	for (auto _ : state)
	{
		SwitchToFiber(pFiber);									// main → fiber → main
	}

	g_stop = true;
	SwitchToFiber(pFiber);
	DeleteFiber(pFiber);
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_Switch_Jc);
BENCHMARK(BM_Switch_Cpp20);
BENCHMARK(BM_Switch_Fiber);

// ========================================================================================
// 3. RoundRobin : M개 코루틴을 1회씩 순회 resume  (iteration 1회 = M 왕복)
// ========================================================================================

static void BM_RoundRobin_Jc(State& state)
{
	EnsureCoVEH();
	const int M = (int)state.range(0);
	g_stop = false;

	std::vector<CoContext*> ctxs(M);
	for (int i = 0; i < M; ++i)
		ctxs[i] = CoRun(fn_YieldForever, cstLow);

	for (auto _ : state)
	{
		for (int i = 0; i < M; ++i)
			ctxs[i] = CoResume(ctxs[i]);
	}

	g_stop = true;
	for (int i = 0; i < M; ++i)
		while (ctxs[i]) ctxs[i] = CoResume(ctxs[i]);

	state.SetItemsProcessed(state.iterations() * M);
}

static void BM_RoundRobin_Cpp20(State& state)
{
	const int M = (int)state.range(0);
	g_stop = false;

	std::vector<StacklessGen> gens;
	gens.reserve(M);
	for (int i = 0; i < M; ++i)
	{
		gens.emplace_back(Gen_YieldForever());
		gens.back().Resume();
	}

	for (auto _ : state)
	{
		for (int i = 0; i < M; ++i)
			gens[i].Resume();
	}

	g_stop = true;
	for (int i = 0; i < M; ++i)
		while (gens[i].Resume()) {}

	state.SetItemsProcessed(state.iterations() * M);
}

static void BM_RoundRobin_Fiber(State& state)
{
	FiberEnv env;
	FiberArg arg{ env.pMain_ };
	const int M = (int)state.range(0);
	g_stop = false;

	std::vector<LPVOID> fibers(M);
	for (int i = 0; i < M; ++i)
		fibers[i] = CreateFiberEx(CO_PAGE_SIZE, CO_STACK_SIZE_LOW, FIBER_FLAG_FLOAT_SWITCH, FiberProc_YieldForever, &arg);

	for (auto _ : state)
	{
		for (int i = 0; i < M; ++i)
			SwitchToFiber(fibers[i]);
	}

	g_stop = true;
	for (int i = 0; i < M; ++i)
	{
		SwitchToFiber(fibers[i]);
		DeleteFiber(fibers[i]);
	}

	state.SetItemsProcessed(state.iterations() * M);
}

// 4096 × cstLow 기준: 예약 64KB 단위(VirtualAlloc granularity) → 주소공간 256MB, 커밋 5페이지(20KB) → 80MB
BENCHMARK(BM_RoundRobin_Jc)->Arg(64)->Arg(1024)->Arg(4096)->Unit(TimeUnit::kMicrosecond);
BENCHMARK(BM_RoundRobin_Cpp20)->Arg(64)->Arg(1024)->Arg(4096)->Unit(TimeUnit::kMicrosecond);
BENCHMARK(BM_RoundRobin_Fiber)->Arg(64)->Arg(1024)->Arg(4096)->Unit(TimeUnit::kMicrosecond);

// ========================================================================================
// 4. DeepYield : 깊이 D 콜스택 끝에서 yield  (iteration 1회 = D 하강 + yield + D 복귀)
//   - jc     : 일반 함수 재귀 D단계 후 CoYield  (프레임 = 네이티브 스택)
//   - Cpp20  : Task 재귀 D단계 (프레임 D개 heap 할당 + symmetric transfer) 후 YieldToDriver
// ========================================================================================

static void BM_DeepYield_Jc(State& state)
{
	EnsureCoVEH();
	g_depth = (int)state.range(0);
	g_stop  = false;

	// depth 256 × ~150B ≈ 40KB → cstHigh(256KB) 사용. 첫 하강 때 VEH 확장이 몇 번 발생하고 이후엔 커밋 유지.
	CoContext* pCtx = CoRun(fn_DeepYieldForever, cstHigh);

	for (auto _ : state)
	{
		pCtx = CoResume(pCtx);
	}

	g_stop = true;
	while (pCtx) pCtx = CoResume(pCtx);
	state.SetItemsProcessed(state.iterations());
}

static void BM_DeepYield_Cpp20(State& state)
{
	g_depth = (int)state.range(0);
	g_stop  = false;

	StacklessTask t = Task_DeepYieldForever();
	t.Start();
	t.Resume();												// 첫 leaf yield 까지 하강

	for (auto _ : state)
	{
		t.Resume();
	}

	g_stop = true;
	while (t.Resume()) {}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_DeepYield_Jc)->Arg(1)->Arg(8)->Arg(64)->Arg(256);
BENCHMARK(BM_DeepYield_Cpp20)->Arg(1)->Arg(8)->Arg(64)->Arg(256);

// ========================================================================================
// 5. StackGrowth : 큰 지역 배열 터치 후 종료  (생성 + 확장 + 해제 전체 비용)
//   - Jc_Lazy  : 기본 설정(init 2p / guard 3p / grow 2p) → VEH 확장 반복
//   - Jc_Eager : pageInitCount = 전체 페이지 → 확장 없음 (선커밋 비용 vs 확장 비용 비교)
//   - Fiber    : commit 4KB, 커널 가드페이지 확장
// ========================================================================================

static void BM_StackGrowth_Jc_Lazy(State& state)
{
	EnsureCoVEH();
	g_touchBytes = (int)state.range(0);
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	for (auto _ : state)
	{
		CoContext* pCtx = CoRun(fn_Touch, cstHigh);
		DoNotOptimize(pCtx);
	}
	state.SetItemsProcessed(state.iterations());
}

static void BM_StackGrowth_Jc_Eager(State& state)
{
	EnsureCoVEH();
	g_touchBytes = (int)state.range(0);
	// 전체를 미리 커밋 (오버플로우 가드 1페이지는 InitStack 이 클램프로 남겨둠)
	g_cCoMgr.SetPageInitCount(CO_STACK_PAGE_COUNT_HIGH);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	for (auto _ : state)
	{
		CoContext* pCtx = CoRun(fn_Touch, cstHigh);
		DoNotOptimize(pCtx);
	}

	// 기본값 복원
	g_cCoMgr.SetPageInitCount(2);
	state.SetItemsProcessed(state.iterations());
}

static void BM_StackGrowth_Fiber(State& state)
{
	FiberEnv env;
	FiberArg arg{ env.pMain_ };
	g_touchBytes = (int)state.range(0);

	for (auto _ : state)
	{
		LPVOID pFiber = CreateFiberEx(CO_PAGE_SIZE, CO_STACK_SIZE_HIGH, FIBER_FLAG_FLOAT_SWITCH, FiberProc_Touch, &arg);
		SwitchToFiber(pFiber);
		DeleteFiber(pFiber);
	}
	state.SetItemsProcessed(state.iterations());
}

// 8KB / 32KB / 128KB  (cstHigh 256KB 내에서 확장. 가드 3p + 오버플로우 1p 를 넘지 않도록 128KB 까지)
BENCHMARK(BM_StackGrowth_Jc_Lazy) ->Arg(8 * 1024)->Arg(32 * 1024)->Arg(128 * 1024)->Unit(TimeUnit::kMicrosecond);
BENCHMARK(BM_StackGrowth_Jc_Eager)->Arg(8 * 1024)->Arg(32 * 1024)->Arg(128 * 1024)->Unit(TimeUnit::kMicrosecond);
BENCHMARK(BM_StackGrowth_Fiber)   ->Arg(8 * 1024)->Arg(32 * 1024)->Arg(128 * 1024)->Unit(TimeUnit::kMicrosecond);

#endif // CoroutineTest == ON
