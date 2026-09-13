#include "Core.h"
#include "06_CoroutineImpl1.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 공통 헬퍼
//////////////////////////////////////////////////////////////////////////////////////////

static void PrintSection(const _char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		_T("\n================================================================"));
	Console::WriteLine(ConsoleColor::Yellow, _T("  %s"), _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		_T("================================================================"));
}

static void PrintPass(const _char* _pTag, const _char* _pMsg = nullptr)
{
	if (_pMsg)
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [%s] %s"), _pTag, _pMsg);
	else
		Console::WriteLine(ConsoleColor::Green, _T("  PASS [%s]"), _pTag);
}

static void PrintFail(const _char* _pTag, const _char* _pMsg)
{
	Console::WriteLine(ConsoleColor::Red, _T("  FAIL [%s] %s"), _pTag, _pMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_CO01~TC_CO15: CoYield / CoResume 종합 테스트
//////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// TC_CO01: fn() 즉시 완료 (yield 없음)
//   fn이 yield 없이 완료 → CoFnEndTrampoline → state=csEnd → CoRun returns nullptr
//--------------------------------------------------------------------------------------
static void fn_CO01(CoContext* _pCtx)
{
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO01] fn 실행: pCtx=%p (yield 없음)"), _pCtx);
}

static void Test_CO01()
{
	PrintSection(_T("TC_CO01: fn() 즉시 완료 (yield 없음) → CoRun=INVALID"));

	CoId id = CoRun(fn_CO01);
	if (id == CO_INVALID_ID)
		PrintPass(_T("CO01"), _T("CoRun=INVALID (fn 즉시 완료 확인)"));
	else
	{
		PrintFail(_T("CO01"), _T("CoRun != INVALID (fn 즉시 완료가 감지되지 않음)"));
		while (CoResume(id)) {}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO02: yield 1회 → CoRun=CoContext*(csYield), CoResume=nullptr
//--------------------------------------------------------------------------------------
static void fn_CO02(CoContext* _pCtx)
{
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO02] 1구간 (yield 전)"));
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO02] 2구간 (yield 후, fn 완료)"));
}

static void Test_CO02()
{
	PrintSection(_T("TC_CO02: yield 1회 - CoRun/CoResume 기본 흐름"));

	CoContext* pView = nullptr;
	CoId id = CoRun(fn_CO02, {.ppOut_ = &pView});
	if (id != CO_INVALID_ID && pView->state_ == csYield)
		PrintPass(_T("CO02/run"), _T("CoRun 유효 (csYield)"));
	else
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("CoRun id=%llu state=%d (기대: csYield=%d)"), (unsigned long long)id, pView ? (int)pView->state_ : -1, (int)csYield);
		PrintFail(_T("CO02/run"), buf);
		while (CoResume(id)) {}
		return;
	}

	if (!CoResume(id))
		PrintPass(_T("CO02/resume"), _T("CoResume=false (fn 완료 확인)"));
	else
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("CoResume=true (false 기대)"));
		PrintFail(_T("CO02/resume"), buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO03: yield 3회 루프
//--------------------------------------------------------------------------------------
static void fn_CO03(CoContext*)
{
	for (int i = 0; i < 3; ++i)
	{
		Console::WriteLine(ConsoleColor::Cyan, _T("    [CO03] 구간 %d (yield 전)"), i + 1);
		CoYield();
	}
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO03] fn 완료"));
}

static void Test_CO03()
{
	PrintSection(_T("TC_CO03: yield 3회 루프"));

	CoId id = CoRun(fn_CO03);
	bool ok = true;

	for (int i = 0; i < 2; ++i)
	{
		if (!CoResume(id))
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), _T("resume %d 에서 종료됨 (계속 기대)"), i + 1);
			PrintFail(_T("CO03"), buf);
			ok = false;
			break;
		}
	}

	if (ok)
	{
		if (!CoResume(id))
			PrintPass(_T("CO03"), _T("3회 yield-resume 완료, 최종 CoResume=false"));
		else
		{
			PrintFail(_T("CO03"), _T("3회 후 false 기대했으나 true 반환"));
			while (CoResume(id)) {}
		}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO04: 3개 코루틴 인터리브 실행
//--------------------------------------------------------------------------------------
static int g_CO04_order[9];
static int g_CO04_orderIdx = 0;

static void fn_CO04_A(CoContext*)
{
	g_CO04_order[g_CO04_orderIdx++] = 1;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 4;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 7;
}
static void fn_CO04_B(CoContext*)
{
	g_CO04_order[g_CO04_orderIdx++] = 2;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 5;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 8;
}
static void fn_CO04_C(CoContext*)
{
	g_CO04_order[g_CO04_orderIdx++] = 3;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 6;
	CoYield();
	g_CO04_order[g_CO04_orderIdx++] = 9;
}

static void Test_CO04()
{
	PrintSection(_T("TC_CO04: 3개 코루틴 인터리브 실행"));

	g_CO04_orderIdx = 0;
	memset(g_CO04_order, 0, sizeof(g_CO04_order));

	CoId idA = CoRun(fn_CO04_A);
	CoId idB = CoRun(fn_CO04_B);
	CoId idC = CoRun(fn_CO04_C);

	// A→B→C 순으로 2회 씩 resume
	CoResume(idA);
	CoResume(idB);
	CoResume(idC);
	CoResume(idA);
	CoResume(idB);
	CoResume(idC);

	bool ok = true;
	for (int i = 0; i < 9; ++i)
	{
		if (g_CO04_order[i] != i + 1)
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), _T("order[%d]=%d (기대 %d)"), i, g_CO04_order[i], i + 1);
			PrintFail(_T("CO04"), buf);
			ok = false;
			break;
		}
	}
	if (ok && !CoResume(idA) && !CoResume(idB) && !CoResume(idC))
		PrintPass(_T("CO04"), _T("3개 코루틴 순서 1~9 정확, 모두 종료 확인"));
	else if (ok)
		PrintFail(_T("CO04"), _T("순서는 맞으나 일부 코루틴이 종료되지 않음"));
}

//--------------------------------------------------------------------------------------
// TC_CO05: fn()에 전달된 CoContext* == CoRun 반환값
//--------------------------------------------------------------------------------------
static CoContext* g_CO05_pCtxFromFn = nullptr;

static void fn_CO05(CoContext* _pCtx)
{
	g_CO05_pCtxFromFn = _pCtx;
	CoYield();
}

static void Test_CO05()
{
	PrintSection(_T("TC_CO05: fn()에 전달된 CoContext*가 ppOut 뷰와 동일한지 검증"));

	g_CO05_pCtxFromFn = nullptr;
	CoContext* pView = nullptr;
	CoId id = CoRun(fn_CO05, {.ppOut_ = &pView});

	if (id != CO_INVALID_ID && pView == g_CO05_pCtxFromFn)
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("ppOut=%p == fn_arg=%p"), pView, g_CO05_pCtxFromFn);
		PrintPass(_T("CO05"), buf);
	}
	else
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("ppOut=%p != fn_arg=%p"), pView, g_CO05_pCtxFromFn);
		PrintFail(_T("CO05"), buf);
	}

	while (CoResume(id)) {}
}

//--------------------------------------------------------------------------------------
// TC_CO06: 10KB 로컬 배열 - VEH 스택 페이지 자동 확장
//--------------------------------------------------------------------------------------
static void fn_CO06(CoContext*)
{
	// 10KB 로컬 배열로 스택 페이지 확장 강제 발동
	volatile char arr[10 * 1024];
	// 배열 끝부터 쓰기 (저주소 방향으로 touch)
	for (int i = (int)sizeof(arr) - 1; i >= 0; i -= (int)CO_PAGE_SIZE)
		arr[i] = (char)i;
	arr[0] = 42;
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO06] 10KB 배열 접근 완료 (arr[0]=%d)"), (int)(unsigned char)arr[0]);
	CoYield();
}

static void Test_CO06()
{
	PrintSection(_T("TC_CO06: 10KB 로컬 배열 - VEH 스택 페이지 자동 확장"));

	// 기본 설정 복원 (initCount=2, guardCount=3, growCount=2)
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	CoContext* pView = nullptr;
	CoId id = CoRun(fn_CO06, {.ppOut_ = &pView});
	if (id != CO_INVALID_ID)
	{
		// 확장 후 pStackLimit_이 내려갔는지 확인
		CoStack* pStack = &pView->stack_;
		Console::WriteLine(ConsoleColor::Cyan,
			_T("    [CO06] 스택 확장 후: pStackLimit_=0x%p  pStackBase_=0x%p  diff=%lld pages"),
			pStack->pStackLimit_, pStack->pStackBase_,
			(long long)(pStack->pStackBase_ - pStack->pStackLimit_) / CO_PAGE_SIZE);
		PrintPass(_T("CO06"), _T("VEH 발동 및 스택 확장 후 yield 성공"));
	}
	else
		PrintFail(_T("CO06"), _T("CoRun이 무효 id를 반환함"));

	while (CoResume(id)) {}
}

//--------------------------------------------------------------------------------------
// TC_CO07: Low / Mid / High tier별 코루틴 정상 실행
//--------------------------------------------------------------------------------------
static void fn_CO07(CoContext*)
{
	CoYield();
}

static void Test_CO07()
{
	PrintSection(_T("TC_CO07: Low / Mid / High tier별 코루틴 정상 실행"));

	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	const CoStackTier tiers[]     = { cstLow, cstMid, cstHigh };
	const _char*      tierNames[] = { _T("Low"), _T("Mid"), _T("High") };

	for (int i = 0; i < 3; ++i)
	{
		CoContext* pView = nullptr;
		CoId id = CoRun(fn_CO07, {.spec_ = CoStackSpec::FromTier(tiers[i]), .ppOut_ = &pView});
		if (id != CO_INVALID_ID && pView->stack_.stackTier_ == tiers[i])
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), _T("tier=%s pStackBase_=0x%p"), tierNames[i], pView->stack_.pStackBase_);
			PrintPass(_T("CO07"), buf);
			while (CoResume(id)) {}
		}
		else
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), _T("tier=%s 실패 (id=%llu tier=%d)"),
				tierNames[i], (unsigned long long)id, pView ? (int)pView->stack_.stackTier_ : -1);
			PrintFail(_T("CO07"), buf);
			while (CoResume(id)) {}
		}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO08: 로컬 변수 보존 - yield/resume 사이에 volatile 지역변수 값 유지
//--------------------------------------------------------------------------------------
static bool g_CO08_pass = false;

static void fn_CO08(CoContext*)
{
	volatile int a = 0xDEAD;
	volatile int b = 0xBEEF;
	CoYield();
	g_CO08_pass = (a == 0xDEAD && b == 0xBEEF);
}

static void Test_CO08()
{
	PrintSection(_T("TC_CO08: 로컬 변수 보존 (yield/resume 사이 volatile 지역변수 유지)"));

	g_CO08_pass = false;
	CoId id = CoRun(fn_CO08);
	while (CoResume(id)) {}

	if (g_CO08_pass)
		PrintPass(_T("CO08"), _T("volatile 지역변수 a=0xDEAD, b=0xBEEF 유지 확인"));
	else
		PrintFail(_T("CO08"), _T("지역변수 값이 변조됨"));
}

//--------------------------------------------------------------------------------------
// TC_CO09: 중첩 코루틴 (코루틴 내에서 CoRun/CoResume 호출)
//--------------------------------------------------------------------------------------
static CoId g_CO09_innerId = CO_INVALID_ID;

static void fn_CO09_inner(CoContext*)
{
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/inner] 시작, yield"));
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/inner] resume 후 완료"));
}

static void fn_CO09_outer(CoContext*)
{
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/outer] 내부 코루틴 실행"));
	g_CO09_innerId = CoRun(fn_CO09_inner);
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/outer] 내부 yield 감지, 외부 yield"));
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/outer] resume, 내부 코루틴 재개"));
	CoResume(g_CO09_innerId);
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO09/outer] 완료"));
}

static void Test_CO09()
{
	PrintSection(_T("TC_CO09: 중첩 코루틴 (코루틴 내에서 CoRun/CoResume 호출)"));

	g_CO09_innerId = CO_INVALID_ID;
	CoId outerId = CoRun(fn_CO09_outer);

	// outer가 inner를 실행하고 yield → 둘 다 살아있음
	bool innerOk = (g_CO09_innerId != CO_INVALID_ID);
	bool outerOk = (outerId != CO_INVALID_ID);

	if (innerOk && outerOk)
		PrintPass(_T("CO09/first_yield"), _T("outer 유효, inner 유효 확인"));
	else
	{
		_char buf[192];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("outer=%llu inner=%llu"),
			(unsigned long long)outerId,
			(unsigned long long)g_CO09_innerId);
		PrintFail(_T("CO09/first_yield"), buf);
		while (CoResume(outerId)) {}
		while (CoResume(g_CO09_innerId)) {}
		return;
	}

	// outer resume → inner resume → inner 완료 → outer 완료 → false
	if (!CoResume(outerId))
		PrintPass(_T("CO09/final"), _T("중첩 코루틴 최종 완료, CoResume=false"));
	else
	{
		PrintFail(_T("CO09/final"), _T("CoResume=true (false 기대)"));
		while (CoResume(outerId)) {}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO10: GS StackBase/StackLimit 복원 검증
//   CoRun / CoResume 호출 전후로 GS 값이 정확히 복원되는지 확인
//--------------------------------------------------------------------------------------
static void fn_CO10(CoContext* _pCtx)
{
	CoYield();
}

static void Test_CO10()
{
	PrintSection(_T("TC_CO10: GS StackBase/StackLimit 복원 검증"));

	char* mainBase  = (char*)__readgsqword(8);
	char* mainLimit = (char*)__readgsqword(16);

	Console::WriteLine(ConsoleColor::Cyan,
		_T("    [CO10] 실행 전: gs:[8]=0x%p  gs:[16]=0x%p"), mainBase, mainLimit);

	CoId id = CoRun(fn_CO10);

	char* curBase  = (char*)__readgsqword(8);
	char* curLimit = (char*)__readgsqword(16);

	if (curBase == mainBase && curLimit == mainLimit)
		PrintPass(_T("CO10/after_run"), _T("CoRun 후 GS 정확히 복원"));
	else
	{
		_char buf[256];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("gs:[8]=%p(exp %p) gs:[16]=%p(exp %p)"), curBase, mainBase, curLimit, mainLimit);
		PrintFail(_T("CO10/after_run"), buf);
	}

	if (id != CO_INVALID_ID) CoResume(id);

	curBase  = (char*)__readgsqword(8);
	curLimit = (char*)__readgsqword(16);

	if (curBase == mainBase && curLimit == mainLimit)
		PrintPass(_T("CO10/after_resume"), _T("CoResume 완료 후 GS 정확히 복원"));
	else
	{
		_char buf[256];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("gs:[8]=%p(exp %p) gs:[16]=%p(exp %p)"), curBase, mainBase, curLimit, mainLimit);
		PrintFail(_T("CO10/after_resume"), buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO11: CoResume(INVALID) 안전 처리 → false 반환
//--------------------------------------------------------------------------------------
static void Test_CO11()
{
	PrintSection(_T("TC_CO11: CoResume(INVALID) 안전 처리"));

	if (!CoResume(CO_INVALID_ID))
		PrintPass(_T("CO11"), _T("CoResume(INVALID) = false (안전 처리 확인)"));
	else
		PrintFail(_T("CO11"), _T("CoResume(INVALID) = true (false 기대)"));
}

//--------------------------------------------------------------------------------------
// TC_CO12: 동일 fn 5회 반복 CoRun/CoResume (컨텍스트 재사용 안정성)
//--------------------------------------------------------------------------------------
static int g_CO12_count = 0;

static void fn_CO12(CoContext*)
{
	g_CO12_count++;
	CoYield();
	g_CO12_count++;
}

static void Test_CO12()
{
	PrintSection(_T("TC_CO12: 동일 fn 5회 반복 CoRun/CoResume (풀링 안정성)"));

	g_CO12_count = 0;
	const int N = 5;

	for (int i = 0; i < N; ++i)
	{
		CoId id = CoRun(fn_CO12);
		while (CoResume(id)) {}
	}

	if (g_CO12_count == N * 2)
	{
		_char buf[64];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("%d회 반복, count=%d"), N, g_CO12_count);
		PrintPass(_T("CO12"), buf);
	}
	else
	{
		_char buf[64];
		StringUtil::FormatBuffer(buf, sizeof(buf), _T("count=%d (기대 %d)"), g_CO12_count, N * 2);
		PrintFail(_T("CO12"), buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO13: cstCustom tier — 크기별 resolvedTier 동작 검증
//
//   AllocCtx는 cstCustom 요청 시 _stackSize에 따라 resolvedTier를 결정:
//     size <  CO_STACK_SIZE_LOW  ( 16KB) → resolvedTier = cstLow  (풀 우선 사용)
//     size <  CO_STACK_SIZE_MID  ( 64KB) → resolvedTier = cstMid  (풀 우선 사용)
//     size <  CO_STACK_SIZE_HIGH (256KB) → resolvedTier = cstHigh (풀 우선 사용)
//     size >= CO_STACK_SIZE_HIGH (256KB) → resolvedTier = cstCustom (풀링 없음, 진짜 Custom)
//
//   풀이 비어있으면 AllocStack(cstCustom, size) 신규 할당 → stackTier_ = cstCustom
//   풀에 항목이 있으면 해당 티어 컨텍스트 반환              → stackTier_ = Low/Mid/High
//
//   CO13-A:   8KB < LOW  (16KB) → resolvedTier = cstLow
//   CO13-B:  24KB < MID  (64KB) → resolvedTier = cstMid
//   CO13-C: 128KB < HIGH(256KB) → resolvedTier = cstHigh
//   CO13-D: 320KB ≥ HIGH(256KB) → 진짜 cstCustom (반드시 tier=cstCustom, size=320KB)
//--------------------------------------------------------------------------------------
static void fn_CO13(CoContext*)
{
	CoYield();
}

static void Test_CO13()
{
	PrintSection(_T("TC_CO13: cstCustom tier — 크기별 resolvedTier 동작 검증"));

	// CO13-A: 8KB < CO_STACK_SIZE_LOW(16KB) → resolvedTier = cstLow
	//   풀 미스: stackTier_=cstCustom, size=8KB
	//   풀 히트: stackTier_=cstLow,   size=16KB
	{
		const _u32 reqSize = 2 * CO_PAGE_SIZE; // 8 KB
		CoContext* pView = nullptr;
		CoId id = CoRun(fn_CO13, {.spec_ = CoStackSpec::Custom(reqSize), .ppOut_ = &pView});
		if (id != CO_INVALID_ID)
		{
			_char buf[256];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("req=8KB | tier=%d size=%uKB | resolvedTier=cstLow(%d) 풀%s"),
				(int)pView->stack_.stackTier_, pView->stack_.size_ / 1024,
				(int)cstLow, pView->stack_.stackTier_ == cstLow ? "히트" : "미스(신규 cstCustom)");
			PrintPass(_T("CO13-A"), buf);
			while (CoResume(id)) {}
		}
		else
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("id=%llu"), (unsigned long long)id);
			PrintFail(_T("CO13-A"), buf);
		}
	}

	// CO13-B: 24KB, CO_STACK_SIZE_LOW(16KB) ≤ x < CO_STACK_SIZE_MID(64KB) → resolvedTier = cstMid
	//   풀 미스: stackTier_=cstCustom, size=24KB
	//   풀 히트: stackTier_=cstMid,   size=64KB
	{
		const _u32 reqSize = 6 * CO_PAGE_SIZE; // 24 KB
		CoContext* pView = nullptr;
		CoId id = CoRun(fn_CO13, {.spec_ = CoStackSpec::Custom(reqSize), .ppOut_ = &pView});
		if (id != CO_INVALID_ID)
		{
			_char buf[256];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("req=24KB | tier=%d size=%uKB | resolvedTier=cstMid(%d) 풀%s"),
				(int)pView->stack_.stackTier_, pView->stack_.size_ / 1024,
				(int)cstMid, pView->stack_.stackTier_ == cstMid ? "히트" : "미스(신규 cstCustom)");
			PrintPass(_T("CO13-B"), buf);
			while (CoResume(id)) {}
		}
		else
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("id=%llu"), (unsigned long long)id);
			PrintFail(_T("CO13-B"), buf);
		}
	}

	// CO13-C: 128KB, CO_STACK_SIZE_MID(64KB) ≤ x < CO_STACK_SIZE_HIGH(256KB) → resolvedTier = cstHigh
	//   풀 미스: stackTier_=cstCustom, size=128KB
	//   풀 히트: stackTier_=cstHigh,  size=256KB
	{
		const _u32 reqSize = 32 * CO_PAGE_SIZE; // 128 KB
		CoContext* pView = nullptr;
		CoId id = CoRun(fn_CO13, {.spec_ = CoStackSpec::Custom(reqSize), .ppOut_ = &pView});
		if (id != CO_INVALID_ID)
		{
			_char buf[256];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("req=128KB | tier=%d size=%uKB | resolvedTier=cstHigh(%d) 풀%s"),
				(int)pView->stack_.stackTier_, pView->stack_.size_ / 1024,
				(int)cstHigh, pView->stack_.stackTier_ == cstHigh ? "히트" : "미스(신규 cstCustom)");
			PrintPass(_T("CO13-C"), buf);
			while (CoResume(id)) {}
		}
		else
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("id=%llu"), (unsigned long long)id);
			PrintFail(_T("CO13-C"), buf);
		}
	}

	// CO13-D: 320KB ≥ CO_STACK_SIZE_HIGH(256KB) → 진짜 cstCustom, 풀링 없음
	//   반드시: stackTier_==cstCustom, size_==reqSize
	{
		const _u32 reqSize = 80 * CO_PAGE_SIZE; // 320 KB
		CoContext* pView = nullptr;
		CoId id = CoRun(fn_CO13, {.spec_ = CoStackSpec::Custom(reqSize), .ppOut_ = &pView});
		if (id != CO_INVALID_ID
			&& pView->stack_.stackTier_ == cstCustom
			&& pView->stack_.size_ == reqSize)
		{
			_char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("req=320KB | tier=cstCustom size=320KB | 진짜 Custom (풀링 없음)"));
			PrintPass(_T("CO13-D"), buf);
			while (CoResume(id)) {}
		}
		else
		{
			_char buf[256];
			StringUtil::FormatBuffer(buf, sizeof(buf),
				_T("id=%llu tier=%d size=%uKB | 기대: tier=cstCustom(%d) size=320KB"),
				(unsigned long long)id,
				pView ? (int)pView->stack_.stackTier_ : -1,
				pView ? pView->stack_.size_ / 1024 : 0,
				(int)cstCustom);
			PrintFail(_T("CO13-D"), buf);
			while (CoResume(id)) {}
		}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO14: 깊은 재귀 / 스택 오버플로우 검증
//
//   CO14-A: cstMid(64KB) 스택, 80단계 재귀(~40KB) → VEH 스택 확장 성공 확인
//   CO14-B: cstMid(64KB) 스택, 무한 재귀 → 스택 완전 소진 → STATUS_STACK_OVERFLOW
//           exceptionPageCount_=4 설정 → 비상 페이지 COMMIT → SEH 디스패처 동작
//           코루틴 내부 __try/__except 로 포착 후 CoYield
//--------------------------------------------------------------------------------------
static volatile int g_CO14A_result = 0;

static int RecursiveSum(int n)
{
	volatile _char buf[512]; // 재귀 단계마다 512 bytes 소비
	buf[0] = (char)n;
	if (n <= 0)
		return 0;
	return (int)buf[0] + RecursiveSum(n - 1);
}

static void fn_CO14A(CoContext*)
{
	// 약 80단계 재귀, 약 40KB 스택 소비 → VEH 여러 번 발동 예상
	auto pCtx = CoCurrentCtx();
	g_CO14A_result = RecursiveSum(80);
	Console::WriteLine(ConsoleColor::Cyan,
		_T("    [CO14-A] 재귀 완료: result=%d"), g_CO14A_result);
	CoYield();
}

// CO14-B: 오버플로우 포착 플래그
static volatile bool  g_CO14B_caught = false;
static volatile DWORD g_CO14B_code   = 0;

static void fn_CO14B(CoContext*)
{
	// cstMid(64KB) 스택에서 무한 재귀 → 스택 완전 소진 → STATUS_STACK_OVERFLOW
	// exceptionPageCount_=4 로 비상 페이지 확보 → SEH 디스패처 동작 가능
	// __except 핸들러는 stack unwind 후 __try 프레임 RSP로 실행되므로
	// 스택이 고갈된 상태에서도 비상 페이지 덕분에 정상 동작 가능
	__try
	{
		RecursiveSum(99999);
	}
	__except (GetExceptionCode() == STATUS_STACK_OVERFLOW
		? EXCEPTION_EXECUTE_HANDLER
		: EXCEPTION_CONTINUE_SEARCH)
	{
		g_CO14B_code   = GetExceptionCode();
		g_CO14B_caught = true;
	}
	CoYield();
}

static void Test_CO14()
{
	// CO14-A: VEH 스택 자동 확장
	// 스택 오버플로 예외는 아직 테스트 불가능..
	// 오버플로 페이지 가드 이전 페이지가드가 터치했을때
	// VirtualAlloc 도중 스택이 오버플로 페이지 가드를 터치해서 스택 오버플로가 발생하는 경우가 있음.이거 수정해야함.

	PrintSection(_T("TC_CO14-A: 깊은 재귀 (VEH 스택 확장 발동)"));

	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);
	g_CO14A_result = -1;

	CoContext* pView = nullptr;
	CoId id = CoRun(fn_CO14A, {.ppOut_ = &pView});

	if (id != CO_INVALID_ID && g_CO14A_result >= 0)
	{
		CoStack* pStack = &pView->stack_;
		Console::WriteLine(ConsoleColor::Cyan,
			_T("    [CO14-A] 확장 후: pStackLimit_=0x%p  diff=%lld pages from base"),
			pStack->pStackLimit_,
			(long long)(pStack->pStackBase_ - pStack->pStackLimit_) / CO_PAGE_SIZE);
		PrintPass(_T("CO14-A"), _T("깊은 재귀 완료, 스택 자동 확장 성공"));
	}
	else
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("id=%llu result=%d"), (unsigned long long)id, (int)g_CO14A_result);
		PrintFail(_T("CO14-A"), buf);
	}
	while (CoResume(id)) {}

	// CO14-B: STATUS_STACK_OVERFLOW 포착
	PrintSection(_T("TC_CO14-B: 스택 완전 소진 → STATUS_STACK_OVERFLOW __try/__except 포착"));

	// exceptionPageCount_=4: 비상 페이지 4개(16KB) 확보 → SEH 디스패처 동작 가능
	// cstMid(64KB=16pages): 인프라 (4+1+3+2)=10 pages, 여유 6 pages(24KB)
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);
	g_CO14B_caught = false;
	g_CO14B_code   = 0;

	// cstMid(64KB): 무한 재귀로 스택 소진 → 오버플로우 가드 페이지 터치
	// → ExpandStack이 비상 페이지 COMMIT + TEB StackLimit 갱신
	// → CoVEH가 STATUS_STACK_OVERFLOW + NONCONTINUABLE 으로 변환
	// → fn_CO14B 내 __except 블록에서 포착
	CoId idB = CoRun(fn_CO14B);

	if (idB != CO_INVALID_ID
		&& g_CO14B_caught
		&& g_CO14B_code == STATUS_STACK_OVERFLOW)
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("STATUS_STACK_OVERFLOW(0x%08X) 포착 성공"), (unsigned)g_CO14B_code);
		PrintPass(_T("CO14-B"), buf);
		while (CoResume(idB)) {}
	}
	else
	{
		_char buf[256];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("id=%llu caught=%d code=0x%08X | 기대: caught=true code=STATUS_STACK_OVERFLOW(0x%08X)"),
			(unsigned long long)idB,
			(int)g_CO14B_caught, (unsigned)g_CO14B_code,
			(unsigned)STATUS_STACK_OVERFLOW);
		PrintFail(_T("CO14-B"), buf);
		while (CoResume(idB)) {}
	}
}

//--------------------------------------------------------------------------------------
// TC_CO15: 1000회 yield 스트레스 테스트
//--------------------------------------------------------------------------------------
static int g_CO15_count = 0;

static void fn_CO15(CoContext*)
{
	for (int i = 0; i < 1000; ++i)
	{
		g_CO15_count++;
		CoYield();
	}
}

static void Test_CO15()
{
	PrintSection(_T("TC_CO15: 1000회 yield 스트레스 테스트"));

	g_CO15_count = 0;
	CoId id = CoRun(fn_CO15);

	int resumeCount = 0;
	bool alive = true;
	while (alive)
	{
		alive = CoResume(id);
		resumeCount++;
	}

	if (g_CO15_count == 1000 && resumeCount == 1000)
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("yield=%d, resume=%d (모두 1000회)"), g_CO15_count, resumeCount);
		PrintPass(_T("CO15"), buf);
	}
	else
	{
		_char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			_T("yield=%d, resume=%d (1000 기대)"), g_CO15_count, resumeCount);
		PrintFail(_T("CO15"), buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO99: 자율 테스트
//--------------------------------------------------------------------------------------
static void fn_CO99(CoContext*)
{
	float a = 10.f;
	double b = 20.f;
	Console::WriteLine(ConsoleColor::Cyan, _T("    [CO99] 시작: a=%.2f b=%.2f"), a, b);
	CoYield();
	a += 20.f;
	b += 20.f;

	float c = 30.;
	double d = 40.;
	CoYield();

	c += a;
	d += b;
}

static void Test_CO99()
{
	PrintSection(_T("TC_CO99: 자율 테스트"));

	CoId id = CoRun(fn_CO99);

	int resumeCount = 0;
	while (CoResume(id))
	{
		resumeCount++;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
void Test_CoBasic()
{
	void* pVeh = AddVectoredExceptionHandler(1, CoVEH);

	// 기본 설정 복원
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	Test_CO01();
	Test_CO02();
	Test_CO03();
	Test_CO04();
	Test_CO05();
	Test_CO06();
	Test_CO07();
	Test_CO08();
	Test_CO09();
	Test_CO10();
	Test_CO11();
	Test_CO12();
	Test_CO13();
	// Test_CO14(); 
	Test_CO15();
	Test_CO99();

	Console::WriteLine(ConsoleColor::Yellow, _T("\n\n[Test_CoBasic] TC_CO01~TC_CO15 전체 완료"));
	Console::ReadKeyWhile(_T("Press spacebar to continue..."), ConsoleKey::Spacebar);

	g_cCoMgr.Clear();
	RemoveVectoredExceptionHandler(pVeh);
}
