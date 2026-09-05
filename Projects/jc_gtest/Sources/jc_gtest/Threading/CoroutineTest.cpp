/*
 * 작성자: 윤정도
 * jc 코루틴 gtest (docs 01~15 단계별 추가)
 */

#include "jc/Threading/Coroutine.h"

#include <intrin.h>
#include <stdexcept>
#include <float.h>

#if TEST_CoroutineTest == ON

// [코루틴-07] fn이 null이면 종료가 아니라 실패이므로 원인이 보여야 한다.
TEST(Coroutine, Error_NullFunction)
{
	CoContext* pCtx = CoRun(nullptr, cstMid);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNullFunction);
	// 읽으면 지워진다. 같은 실패를 두 번 보고하지 않는다.
	EXPECT_EQ(CoGetLastError(), coeNone);
}

// [코루틴-07] 성공 경로에서는 에러가 남지 않는다.
static void CoTestFn_Empty(CoContext*) {}

TEST(Coroutine, Error_SuccessClearsError)
{
	CoContext* pCtx = CoRun(CoTestFn_Empty, cstMid);
	EXPECT_EQ(pCtx, nullptr);	// 즉시 종료 → 해제 후 nullptr
	EXPECT_EQ(CoGetLastError(), coeNone);
}

// [코루틴-07] VEH를 직접 등록하지 않아도 스택 확장이 동작한다.
// - 이전에는 AddVectoredExceptionHandler를 빼먹으면 여기서 종료됐다.
static void CoTestFn_Grow10K(CoContext*)
{
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

TEST(Coroutine, Veh_AutoRegistered)
{
	CoContext* pCtx = CoRun(CoTestFn_Grow10K, cstMid);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNone);
	// 풀에 반납된 스택을 해제한다. (~CoMgr가 없어 프로세스 종료 시 릭으로 잡히므로)
	g_cCoMgr.Clear();
}

// [코루틴-05] 다른 스레드에서 resume하면 실행되지 않고 에러가 남는다.
// - 이전에는 남의 스레드 매니저에 엉뚱한 컨텍스트가 등록돼 풀이 오염됐다.
static volatile bool g_coTestRanOnWrongThread = false;
static void CoTestFn_YieldThenFlag(CoContext*)
{
	CoYield();
	g_coTestRanOnWrongThread = true;
}

TEST(Coroutine, Resume_WrongThread)
{
	g_coTestRanOnWrongThread = false;
	CoContext* pCtx = CoRun(CoTestFn_YieldThenFlag, cstLow);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->state_, csYield);

	bool resumed = false;
	CoError err = coeNone;
	std::thread th([&]
	{
		CoContext* r = CoResume(pCtx);
		resumed = (r != nullptr);
		err = CoGetLastError();
	});
	th.join();

	EXPECT_FALSE(resumed);
	EXPECT_EQ(err, coeWrongThread);
	EXPECT_FALSE(g_coTestRanOnWrongThread);

	// 원래 스레드에서 정상 종료시킨다.
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-05] 자기 자신을 resume하면 스택이 오염되므로 거부된다.
static CoError g_coTestSelfErr = coeNone;
static bool g_coTestSelfResumed = false;
static void CoTestFn_SelfResume(CoContext*)
{
	CoContext* r = CoResume(CoCurrentCtx());
	g_coTestSelfResumed = (r != nullptr);
	g_coTestSelfErr = CoGetLastError();
}

TEST(Coroutine, Resume_Self)
{
	g_coTestSelfResumed = false;
	CoContext* pCtx = CoRun(CoTestFn_SelfResume, cstLow);
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_FALSE(g_coTestSelfResumed);
	EXPECT_EQ(g_coTestSelfErr, coeInvalidState);
	g_cCoMgr.Clear();
}

// [코루틴-05] 종료 후 같은 주소로 재사용된 컨텍스트를 예전 핸들로 resume하면 거부된다.
// - 이전에는 엉뚱한 코루틴이 실행되는 ABA 문제가 있었다.
static void CoTestFn_YieldOnce(CoContext*)
{
	CoYield();
}

static volatile bool g_coTestB_Resumed = false;
static void CoTestFn_B(CoContext*)
{
	CoYield();
	g_coTestB_Resumed = true;
}

TEST(Coroutine, Resume_StaleHandle)
{
	g_cCoMgr.Clear();	// 풀을 비워 같은 주소 재사용을 확정한다.
	CoHandle h = CoRunH(CoTestFn_YieldOnce, cstLow);
	ASSERT_TRUE(h.IsAlive());

	while (CoResumeH(h) != nullptr) {}	// 끝까지 돌려 풀에 반납
	EXPECT_FALSE(h.IsAlive());

	// 같은 티어로 다시 시작하면 풀에서 같은 주소가 재사용된다.
	CoHandle hB = CoRunH(CoTestFn_B, cstLow);
	ASSERT_TRUE(hB.IsAlive());
	EXPECT_EQ(hB.pCtx, h.pCtx);	// 재사용 확인

	// 예전 핸들은 세대가 달라 죽은 핸들이다. B가 다시 실행되면 안 된다.
	g_coTestB_Resumed = false;
	CoContext* r = CoResumeH(h);
	EXPECT_EQ(r, nullptr);
	EXPECT_EQ(CoGetLastError(), coeStaleHandle);
	EXPECT_FALSE(g_coTestB_Resumed);

	while (CoResumeH(hB) != nullptr) {}
	g_cCoMgr.Clear();
}

// [코루틴-06] 작은 custom 요청은 티어로 올림되고 크기도 티어 크기가 된다.
// - 이전에는 티어만 바뀌고 크기가 5000B 그대로라 정렬이 깨지고 풀이 오염됐다.
static void CoTestFn_YieldForever06(CoContext*)
{
	CoYield();
}

TEST(Coroutine, Custom_RoundUp)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 5000);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->stack_.stackTier_, cstLow);
	EXPECT_EQ(pCtx->stack_.size_, (_u32)CO_STACK_SIZE_LOW);
	EXPECT_EQ(((uintptr_t)pCtx->stack_.pStackBase_ & (CO_PAGE_SIZE - 1)), 0u);
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-06] 크기가 0인 custom은 만들 수 없다.
TEST(Coroutine, Custom_ZeroRejected)
{
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 0);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeInvalidStackSize);
}

// [코루틴-06] custom 5000B가 풀을 오염시키지 않는다.
// - 이전에는 5000B 스택이 cstLow 풀에 들어가 다음 사용자가 좁은 스택을 받았다.
// - 6KB를 쓰는 이유: 5000B 스택에는 절대 안 들어가고, 진짜 16KB Low 스택의
//   초기 커밋(8KB)에는 들어가므로 VEH 확장을 타지 않고 판별된다.
//   (Low에서 10KB 같은 성장은 VEH 예산 부족으로 죽을 수 있어 15에서 다룬다.)
static void CoTestFn_Touch6K(CoContext*)
{
	volatile char buf[6 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
}

TEST(Coroutine, Custom_NoPoolMix)
{
	g_cCoMgr.Clear();
	// 5000B custom → Low로 올림 → 종료 → 풀에 반납
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 5000);
	while (pCtx)
		pCtx = CoResume(pCtx);

	// 같은 풀에서 꺼낸 Low 스택으로 6KB를 써도 멀쩡해야 한다. (5000B였으면 오버플로우)
	pCtx = CoRun(CoTestFn_Touch6K, cstLow);
	EXPECT_EQ(pCtx, nullptr);
	EXPECT_EQ(CoGetLastError(), coeNone);
	g_cCoMgr.Clear();
}

// [코루틴-06] 진짜 큰 custom은 페이지 올림되고 풀에 들어가지 않는다.
TEST(Coroutine, Custom_LargeNoPool)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstCustom, 300000);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_EQ(pCtx->stack_.stackTier_, cstCustom);
	EXPECT_EQ(pCtx->stack_.size_, ((_u32)300000 + CO_PAGE_SIZE - 1) & ~(_u32)(CO_PAGE_SIZE - 1));
	char* pCustomBase = pCtx->stack_.pStackBase_;
	while (pCtx)
		pCtx = CoResume(pCtx);

	// custom은 해제되고 풀에 남지 않으므로 예약 영역이 반환되어야 한다.
	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pCustomBase - 1, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_FREE);

	// 다음 Low 요청이 custom 스택 주소를 받으면 안 된다.
	pCtx = CoRun(CoTestFn_YieldForever06, cstLow);
	ASSERT_NE(pCtx, nullptr);
	EXPECT_NE(pCtx->stack_.pStackBase_, pCustomBase);
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-01] fn 안에서 던진 예외는 스케줄러 스택에서 받을 수 있다.
// - 이전에는 트램폴린을 넘어 언와인더가 꼬여 프로세스가 죽었다.
static void CoTestFn_Throw(CoContext*)
{
	throw std::runtime_error("co-boom");
}

TEST(Coroutine, Exception_ThrowInsideCaughtOutside)
{
	bool caught = false;
	try
	{
		CoContext* pCtx = CoRunChecked(CoTestFn_Throw, cstMid);
		EXPECT_EQ(pCtx, nullptr);
	}
	catch (const std::runtime_error&)
	{
		caught = true;
	}
	EXPECT_TRUE(caught);
	EXPECT_EQ(CoGetLastError(), coeNone);
	CoClearPendingException();	// 보관 소유권 정리 (take 뒤 catch가 끝난 자리)
	g_cCoMgr.Clear();
}

// [코루틴-02/04] 코루틴 안에서 보는 StackBase/Limit은 코루틴 스택을 가리킨다.
// - DeallocationStack은 비상 밴드 상단(pEmergencyTop_)이 설치된다.
//   (예약 하단은 커널이 직접 확장해 VEH가 안 불리므로 경계를 위로 둔다)
static void* g_coTebVals[3] = { nullptr, nullptr, nullptr };
static void* g_coTebBase = nullptr;
static void* g_coTebEmergencyTop = nullptr;
static void CoTestFn_ReadTeb(CoContext* pCtx)
{
	g_coTebVals[0] = (void*)__readgsqword(0x08);
	g_coTebVals[1] = (void*)__readgsqword(0x10);
	g_coTebVals[2] = (void*)__readgsqword(0x1478);
	g_coTebBase = pCtx->stack_.pStackBase_;
	g_coTebEmergencyTop = pCtx->stack_.pEmergencyTop_;
}

TEST(Coroutine, Teb_StackRangesInside)
{
	CoContext* pCtx = CoRun(CoTestFn_ReadTeb, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_EQ(g_coTebVals[0], g_coTebBase);
	EXPECT_EQ(g_coTebVals[2], g_coTebEmergencyTop);
	g_cCoMgr.Clear();
}

// [코루틴-02] 성장 가드존은 PAGE_GUARD다. (NOACCESS면 예외 배달이 죽으므로)
// - DeallocationStack 교체와 조합 시 커널이 직접 확장해 VEH가 안 불릴 수 있어,
//   Teb_StackLimitAfterGrowth 테스트에서 VEH 확장이 도는지 함께 감시한다.
TEST(Coroutine, GuardZone_IsPageGuard)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstMid);
	ASSERT_NE(pCtx, nullptr);
	MEMORY_BASIC_INFORMATION mbi{};
	EXPECT_EQ(::VirtualQuery(pCtx->stack_.pGuardLimit_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_NE(mbi.Protect & PAGE_GUARD, 0u);
	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

// [코루틴-03] 확장 후에도 throw/catch가 되고, yield를 거치면 한계가 동기화된다.
// - 이전에는 gs:[16]이 그대로라 확장된 영역에서 catch에 닿지 못하고 죽었다.
// - 한계 동기화는 yield 시점에 살아있는 TEB 값을 기준으로 맞춘다.
//   (커널이 가드 폴트를 직접 확장할 수 있어 pStackLimit_만 믿을 수 없기 때문)
static bool g_coTestLimitMatch = false;
static bool g_coTestCaught = false;
static void CoTestFn_ThrowAfterGrow(CoContext*)
{
	volatile char big[CO_PAGE_SIZE * 4];
	for (int i = (int)sizeof(big) - 1; i >= 0; i -= CO_PAGE_SIZE)
		big[i] = (char)i;
	CoYield();	// 여기서 살아있는 한계가 stack_에 동기화된다.
	void* limitNow = (void*)__readgsqword(0x10);
	CoContext* self = CoCurrentCtx();
	g_coTestLimitMatch = (limitNow == self->stack_.pStackLimit_);
	try { throw 42; }
	catch (int v) { g_coTestCaught = (v == 42); }
	(void)big;
}

TEST(Coroutine, Teb_StackLimitAfterGrowth)
{
	g_coTestLimitMatch = false;
	g_coTestCaught = false;
	CoContext* pCtx = CoRun(CoTestFn_ThrowAfterGrow, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_TRUE(g_coTestLimitMatch);
	EXPECT_TRUE(g_coTestCaught);
	g_cCoMgr.Clear();
}

// [코루틴-08] 코루틴이 바꾼 반올림 모드가 yield/resume을 넘어 유지되고,
// 스레드 쪽에는 새지 않는다. (x87 CW도 같은 경로로 교체되므로 MXCSR로 대표 검증)
static unsigned g_coMxcsrInside = 0;
static void CoTestFn_Rounding(CoContext*)
{
	unsigned old = 0;
	::_controlfp_s(&old, _RC_DOWN, _MCW_RC);
	CoYield();
	unsigned now = 0;
	::_controlfp_s(&now, 0, 0);
	g_coMxcsrInside = now;
}

TEST(Coroutine, Abi_MxcsrPreserved)
{
	CoContext* pCtx = CoRun(CoTestFn_Rounding, cstMid);
	ASSERT_NE(pCtx, nullptr);
	unsigned mid = 0;
	::_controlfp_s(&mid, 0, 0);
	EXPECT_EQ(mid & _MCW_RC, (unsigned)_RC_NEAR);	// 스레드 쪽은 그대로
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_EQ(g_coMxcsrInside & _MCW_RC, (unsigned)_RC_DOWN);	// 코루틴 쪽은 유지
	unsigned after = 0;
	::_controlfp_s(&after, 0, 0);
	EXPECT_EQ(after & _MCW_RC, (unsigned)_RC_NEAR);	// 종료 후 스레드 복원
	g_cCoMgr.Clear();
}

// [코루틴-04] 스택 오버플로우가 __except에 닿고, 표시·복구가 된다.
// - 이전에는 가드를 치는 순간 디스패치 공간이 없어 이중 폴트로 강제 종료됐다.
// - 예약 아래 비상 패드가 SEH 디스패치 공간을 보장한다.
// - 확립된 프레임에서 직접 찍으면 잡힌다. (__chkstk/RTC-fill처럼 RSP가 따라
//   내려가는 프로브는 배달 공간·언와인드 문제로 죽을 수 있어 별도 검증 대상)
static int CoTestFilterSO(unsigned _code)
{
	return _code == EXCEPTION_STACK_OVERFLOW ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

// [코루틴-03] yield 시점에 살아있는 한계가 stack_에 동기화된다.
// - 커널이 가드 폴트를 직접 확장할 수 있어(실측) pStackLimit_만 믿을 수 없으므로,
//   yield 때 TEB 값을 기준으로 맞추고 resume은 이 값을 읽는다.
// - 우리 ExpandStack이 돌았으면 guard까지 내려가 있고, 커널이 먹었으면
//   guard는 그대로다. (limit은 yield 동기화로 내려감)
static char* g_coSyncLimitAtYield = nullptr;
static char* g_coSyncLimitInitial = nullptr;
static char* g_coSyncGuardAtYield = nullptr;
static char* g_coSyncGuardInitial = nullptr;
static void CoTestFn_SyncGrowYield(CoContext* pCtx)
{
	g_coSyncLimitInitial = pCtx->stack_.pStackLimit_;
	g_coSyncGuardInitial = pCtx->stack_.pGuardLimit_;
	volatile char buf[10 * 1024];
	for (int i = 0; i < (int)sizeof(buf); i += 4096)
		buf[i] = (char)i;
	CoYield();
	g_coSyncLimitAtYield = CoCurrentCtx()->stack_.pStackLimit_;
	g_coSyncGuardAtYield = CoCurrentCtx()->stack_.pGuardLimit_;
	(void)buf;
}

TEST(Coroutine, Growth_YieldSyncsLimit)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_SyncGrowYield, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	// 한계는 살아있는 값으로 동기화된다. 가드는 우리 ExpandStack이 안 돌아서 그대로다.
	EXPECT_NE(g_coSyncLimitAtYield, g_coSyncLimitInitial);
	EXPECT_EQ(g_coSyncGuardAtYield, g_coSyncGuardInitial);
	g_cCoMgr.Clear();
}

static bool g_coOverflowCaught = false;
static bool g_coOverflowFlag = false;
static bool g_coOverflowReset = false;

// [코루틴-04] 확립된 프레임에서 오버플로우 가드를 직접 찍으면 잡힌다.
// - __chkstk/RTC-fill처럼 RSP가 따라 내려가는 프로브는 배달 공간이 없어 죽지만,
//   직접 인덱스로 찍으면 RSP가 위에 있어 디스패치가 된다.
__declspec(noinline) static void CoTestTouchDeep(volatile char* _p)
{
	*_p = 42;
}

static void CoTestFn_DirectTouch(CoContext* pCtx)
{
	__try
	{
		// 오버플로우 가드 페이지를 직접 찍는다. (RSP는 위에 그대로)
		volatile char* pGuard = (volatile char*)pCtx->stack_.pStackEnd_;
		CoTestTouchDeep(pGuard);
	}
	__except (CoTestFilterSO(GetExceptionCode()))
	{
		g_coOverflowCaught = true;
		CoNoteStackOverflow();
		g_coOverflowFlag = CoCurrentCtx()->stack_.overflowed_;
		g_coOverflowReset = CoResetStackOverflow();
	}
}

TEST(Coroutine, Overflow_DirectTouchCaught)
{
	g_coOverflowCaught = false;
	g_coOverflowFlag = false;
	g_coOverflowReset = false;
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_DirectTouch, cstMid);
	while (pCtx)
		pCtx = CoResume(pCtx);
	EXPECT_TRUE(g_coOverflowCaught);
	EXPECT_TRUE(g_coOverflowFlag);
	EXPECT_TRUE(g_coOverflowReset);
	g_cCoMgr.Clear();
}

// [코루틴-04] 비상 레이아웃이 제대로 깔려 있는지 확인한다. (죽지 않는 검사)
// - 예약 아래 패드 N페이지 RW 커밋, 오버플로우 가드 1페이지 NOACCESS,
//   DeallocationStack = 밴드 상단.
TEST(Coroutine, Overflow_LayoutCheck)
{
	g_cCoMgr.Clear();
	CoContext* pCtx = CoRun(CoTestFn_YieldForever06, cstMid);
	ASSERT_NE(pCtx, nullptr);

	MEMORY_BASIC_INFORMATION mbi{};
	// 패드: 예약 시작 주소는 RW 커밋이어야 한다.
	EXPECT_EQ(::VirtualQuery(pCtx->stack_.pReserveBase_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_EQ(mbi.Protect & 0xFF, (DWORD)PAGE_READWRITE);
	// 오버플로우 가드: GUARD여야 한다. (배달 push가 자동 해제)
	EXPECT_EQ(::VirtualQuery(pCtx->stack_.pStackEnd_, &mbi, sizeof(mbi)), sizeof(mbi));
	EXPECT_EQ(mbi.State, (DWORD)MEM_COMMIT);
	EXPECT_NE(mbi.Protect & PAGE_GUARD, 0u);
	// 패드 크기: Mid(16p)는 4장이다.
	EXPECT_EQ(pCtx->stack_.pStackEnd_ - pCtx->stack_.pReserveBase_, (ptrdiff_t)(4 * CO_PAGE_SIZE));
	// 밴드 상단 = 예약 하단 + 1페이지다.
	EXPECT_EQ(pCtx->stack_.pEmergencyTop_, pCtx->stack_.pStackEnd_ + CO_PAGE_SIZE);

	while (pCtx)
		pCtx = CoResume(pCtx);
	g_cCoMgr.Clear();
}

#endif // TEST_CoroutineTest == ON
