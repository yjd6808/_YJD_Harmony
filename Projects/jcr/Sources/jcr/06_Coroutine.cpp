#include "Core.h"
#include "06_CoroutineImpl1.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 공통 헬퍼
//////////////////////////////////////////////////////////////////////////////////////////

static void PrintSection(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

static void PrintPass(const char* _pTag, const char* _pMsg = nullptr)
{
	if (_pMsg)
		Console::WriteLine(ConsoleColor::Green, "  PASS [%s] %s", _pTag, _pMsg);
	else
		Console::WriteLine(ConsoleColor::Green, "  PASS [%s]", _pTag);
}

static void PrintFail(const char* _pTag, const char* _pMsg)
{
	Console::WriteLine(ConsoleColor::Red, "  FAIL [%s] %s", _pTag, _pMsg);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스택 포인터 값 검증 (TC_StackGrowth 에서 계속 사용)
//////////////////////////////////////////////////////////////////////////////////////////

static bool VerifyPtrs(
	CoStack*	_pStack,
	char*		_expLimit,
	char*		_expGuardLimit,
	const char*	_pTag)
{
	bool ok = true;
	if (_pStack->pStackLimit_ != _expLimit)
	{
		Console::WriteLine(ConsoleColor::Red,
			"  FAIL [%s] pStackLimit_: 0x%p  (expected 0x%p, diff %+lld pages)",
			_pTag,
			_pStack->pStackLimit_,
			_expLimit,
			(long long)(_pStack->pStackLimit_ - _expLimit) / CO_PAGE_SIZE);
		ok = false;
	}
	if (_pStack->pGuardLimit_ != _expGuardLimit)
	{
		Console::WriteLine(ConsoleColor::Red,
			"  FAIL [%s] pGuardLimit_: 0x%p  (expected 0x%p, diff %+lld pages)",
			_pTag,
			_pStack->pGuardLimit_,
			_expGuardLimit,
			(long long)(_pStack->pGuardLimit_ - _expGuardLimit) / CO_PAGE_SIZE);
		ok = false;
	}
	if (ok)
	{
		Console::WriteLine(ConsoleColor::Green,
			"  PASS [%s] pStackLimit_=0x%p  pGuardLimit_=0x%p",
			_pTag, _pStack->pStackLimit_, _pStack->pGuardLimit_);
	}
	return ok;
}

static bool VerifyPageState(
	char*		_pAddr,
	DWORD		_expState,
	DWORD		_expProtectMask,
	const char*	_pTag)
{
	MEMORY_BASIC_INFORMATION mbi = {};
	VirtualQuery(_pAddr, &mbi, sizeof(mbi));

	bool stateOk   = (mbi.State == _expState);
	bool protectOk = (_expProtectMask == 0) || ((mbi.Protect & _expProtectMask) != 0);

	if (!stateOk || !protectOk)
	{
		Console::WriteLine(ConsoleColor::Red,
			"  FAIL [%s] Addr=0x%p  State=0x%08X(exp 0x%08X)  Protect=0x%08X(expMask 0x%08X)",
			_pTag, _pAddr, mbi.State, _expState, mbi.Protect, _expProtectMask);
		return false;
	}

	Console::WriteLine(ConsoleColor::Green,
		"  PASS [%s] Addr=0x%p  State=0x%08X  Protect=0x%08X",
		_pTag, _pAddr, mbi.State, mbi.Protect);
	return true;
}

static bool VerifyGuardPages(CoStack* _pStack, _u32 _expGuardCount, const char* _pTag)
{
	_u32 actualGuardCount = (_u32)((_pStack->pStackLimit_ - _pStack->pGuardLimit_) / CO_PAGE_SIZE);
	if (actualGuardCount != _expGuardCount)
	{
		Console::WriteLine(ConsoleColor::Red,
			"  FAIL [%s] guardPageCount: %u  (expected %u)",
			_pTag, actualGuardCount, _expGuardCount);
		return false;
	}

	bool ok = true;

	for (_u32 i = 0; i < _expGuardCount; ++i)
	{
		char* pPage = _pStack->pGuardLimit_ + i * CO_PAGE_SIZE;
		char tagBuf[128];
		StringUtil::FormatBuffer(tagBuf, sizeof(tagBuf), "%s/GUARD[%u]", _pTag, i);
		if (!VerifyPageState(pPage, MEM_COMMIT, PAGE_GUARD, tagBuf))
			ok = false;
	}

	if (_expGuardCount == 0 && _pStack->pGuardLimit_ == _pStack->pStackLimit_)
	{
		Console::WriteLine(ConsoleColor::Cyan,
			"  INFO [%s] 가드 페이지 0개 - pGuardLimit_ == pStackLimit_ (스택 바닥 도달)", _pTag);
	}

	char* pCommitPage = _pStack->pStackLimit_;
	char tagCommit[128];
	StringUtil::FormatBuffer(tagCommit, sizeof(tagCommit), "%s/COMMIT_above_limit", _pTag);
	if (!VerifyPageState(pCommitPage, MEM_COMMIT, PAGE_READWRITE, tagCommit))
		ok = false;

	if (ok)
	{
		Console::WriteLine(ConsoleColor::Green,
			"  PASS [%s] guard=%u pages  pStackLimit_=0x%p  pGuardLimit_=0x%p",
			_pTag, _expGuardCount, _pStack->pStackLimit_, _pStack->pGuardLimit_);
	}
	return ok;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_StackGrowth: pageGrowCount_ 변화 테스트 (TC01~TC03)
//////////////////////////////////////////////////////////////////////////////////////////
static void TC_GrowCount(const char* _pName, _u32 _growCount, _u32 _expLimitDelta, _u32 _expGuardDelta)
{
	g_cCoMgr.Configure(2, 3, _growCount);

	CoContext* pCtx = g_cCoMgr.AllocCtx(cstMid, 0);
	CoStack*   pStack = &pCtx->stack_;

	char* pInitLimit      = pStack->pStackLimit_;
	char* pInitGuardLimit = pStack->pGuardLimit_;

	char tagPre[64];
	StringUtil::FormatBuffer(tagPre, sizeof(tagPre), "%s/pre", _pName);
	VerifyPageState(pInitLimit - CO_PAGE_SIZE, MEM_COMMIT, PAGE_GUARD, tagPre);

	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;

	char* expLimit      = pInitLimit      - _expLimitDelta * CO_PAGE_SIZE;
	char* expGuardLimit = pInitGuardLimit - _expGuardDelta * CO_PAGE_SIZE;

	char tagPtrs[64];
	StringUtil::FormatBuffer(tagPtrs, sizeof(tagPtrs), "%s/ptrs", _pName);
	VerifyPtrs(pStack, expLimit, expGuardLimit, tagPtrs);

	char tagGuard[64];
	StringUtil::FormatBuffer(tagGuard, sizeof(tagGuard), "%s/guard", _pName);
	VerifyGuardPages(pStack, 3, tagGuard);

	g_cCoMgr.DumpStack(pStack, _pName);
	g_cCoMgr.FreeCtx(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_StackGrowth: 가드 페이지 소진 테스트 (TC04~TC07)
//////////////////////////////////////////////////////////////////////////////////////////
static void TC04_to_TC07_GuardExhaustion()
{
	PrintSection("TC04~TC07: 가드 페이지 설치 가능 수 소진 테스트 (growCount=0, High tier 64-pages)");

	g_cCoMgr.Configure(60, 3, 0);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstHigh, 0);
	CoStack*   pStack = &pCtx->stack_;

	char* pEnd = pStack->pStackEnd_;

	Console::WriteLine(ConsoleColor::Cyan, "\n[초기 상태]");
	VerifyPtrs(pStack, pEnd + 4 * CO_PAGE_SIZE, pEnd + 1 * CO_PAGE_SIZE, "TC04~07/init");
	VerifyPageState(pEnd, MEM_RESERVE, 0, "TC04~07/init/bottom_reserve");
	VerifyGuardPages(pStack, 3, "TC04~07/init");

	PrintSection("TC04: 1번째 터치 - 가드 3개 설치 (정상)");
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;
	VerifyPtrs(pStack, pEnd + 3 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC04/ptrs");
	VerifyGuardPages(pStack, 3, "TC04/guard");
	VerifyPageState(pEnd + 3 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC04/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC04 After");

	PrintSection("TC05: 2번째 터치 - 가드 2개만 설치 가능");
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;
	VerifyPtrs(pStack, pEnd + 2 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC05/ptrs");
	VerifyGuardPages(pStack, 2, "TC05/guard");
	VerifyPageState(pEnd + 2 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC05/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC05 After");

	PrintSection("TC06: 3번째 터치 - 가드 1개만 설치 가능");
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;
	VerifyPtrs(pStack, pEnd + 1 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC06/ptrs");
	VerifyGuardPages(pStack, 1, "TC06/guard");
	VerifyPageState(pEnd + 1 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC06/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC06 After");

	PrintSection("TC07: 4번째 터치 - 가드 0개 (바닥 도달)");
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;
	VerifyPtrs(pStack, pEnd + 0 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC07/ptrs");
	VerifyGuardPages(pStack, 0, "TC07/guard");
	VerifyPageState(pEnd + 0 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC07/commit_at_limit");
	g_cCoMgr.DumpStack(pStack, "TC07 After");

	g_cCoMgr.FreeCtx(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_CoStack()
{
	void* pVeh = AddVectoredExceptionHandler(1, CoVEH);

	PrintSection("TC01: pageGrowCount_=0 (추가 grow 없음)");
	TC_GrowCount("TC01", 0, 1, 1);

	PrintSection("TC02: pageGrowCount_=1 (1 페이지 추가 grow)");
	TC_GrowCount("TC02", 1, 2, 2);

	PrintSection("TC03: pageGrowCount_=2 (2 페이지 추가 grow, 기본값)");
	TC_GrowCount("TC03", 2, 3, 3);

	TC04_to_TC07_GuardExhaustion();

	Console::WriteLine(ConsoleColor::Yellow, "\n\n[Test_CoStack] 전체 테스트 완료");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	g_cCoMgr.Clear();
	RemoveVectoredExceptionHandler(pVeh);
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
	Console::WriteLine(ConsoleColor::Cyan, "    [CO01] fn 실행: pCtx=%p (yield 없음)", _pCtx);
}

static void Test_CO01()
{
	PrintSection("TC_CO01: fn() 즉시 완료 (yield 없음) → CoRun=nullptr");

	CoContext* pCtx = CoRun(fn_CO01);
	if (pCtx == nullptr)
		PrintPass("CO01", "CoRun=nullptr (fn 즉시 완료 확인)");
	else
	{
		PrintFail("CO01", "CoRun != nullptr (fn 즉시 완료가 감지되지 않음)");
		g_cCoMgr.FreeCtx(pCtx);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO02: yield 1회 → CoRun=CoContext*(csYield), CoResume=nullptr
//--------------------------------------------------------------------------------------
static void fn_CO02(CoContext* _pCtx)
{
	Console::WriteLine(ConsoleColor::Cyan, "    [CO02] 1구간 (yield 전)");
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, "    [CO02] 2구간 (yield 후, fn 완료)");
}

static void Test_CO02()
{
	PrintSection("TC_CO02: yield 1회 - CoRun/CoResume 기본 흐름");

	CoContext* pCtx = CoRun(fn_CO02);
	if (pCtx != nullptr && pCtx->state_ == csYield)
		PrintPass("CO02/run", "CoRun=CoContext*(csYield)");
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoRun=%p state=%d (기대: csYield=%d)", pCtx, pCtx ? (int)pCtx->state_ : -1, (int)csYield);
		PrintFail("CO02/run", buf);
		if (pCtx) g_cCoMgr.FreeCtx(pCtx);
		return;
	}

	CoContext* pResult = CoResume(pCtx);
	if (pResult == nullptr)
		PrintPass("CO02/resume", "CoResume=nullptr (fn 완료 확인)");
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoResume=%p (nullptr 기대)", pResult);
		PrintFail("CO02/resume", buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO03: yield 3회 루프
//--------------------------------------------------------------------------------------
static void fn_CO03(CoContext*)
{
	for (int i = 0; i < 3; ++i)
	{
		Console::WriteLine(ConsoleColor::Cyan, "    [CO03] 구간 %d (yield 전)", i + 1);
		CoYield();
	}
	Console::WriteLine(ConsoleColor::Cyan, "    [CO03] fn 완료");
}

static void Test_CO03()
{
	PrintSection("TC_CO03: yield 3회 루프");

	CoContext* pCtx = CoRun(fn_CO03);
	bool ok = true;

	for (int i = 0; i < 3; ++i)
	{
		if (pCtx == nullptr || pCtx->state_ != csYield)
		{
			char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), "resume %d 에서 pCtx=%p state=%d (csYield=%d 기대)", i + 1, pCtx, pCtx ? (int)pCtx->state_ : -1, (int)csYield);
			PrintFail("CO03", buf);
			if (pCtx) g_cCoMgr.FreeCtx(pCtx);
			return;
		}
		pCtx = CoResume(pCtx);
	}

	if (pCtx == nullptr)
		PrintPass("CO03", "3회 yield-resume 완료, 최종 CoResume=nullptr");
	else
	{
		PrintFail("CO03", "3회 후 nullptr 기대했으나 pCtx != nullptr");
		g_cCoMgr.FreeCtx(pCtx);
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
	PrintSection("TC_CO04: 3개 코루틴 인터리브 실행");

	g_CO04_orderIdx = 0;
	memset(g_CO04_order, 0, sizeof(g_CO04_order));

	CoContext* pA = CoRun(fn_CO04_A);
	CoContext* pB = CoRun(fn_CO04_B);
	CoContext* pC = CoRun(fn_CO04_C);

	// A→B→C 순으로 2회 씩 resume
	pA = CoResume(pA);
	pB = CoResume(pB);
	pC = CoResume(pC);
	pA = CoResume(pA);
	pB = CoResume(pB);
	pC = CoResume(pC);

	bool ok = true;
	for (int i = 0; i < 9; ++i)
	{
		if (g_CO04_order[i] != i + 1)
		{
			char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), "order[%d]=%d (기대 %d)", i, g_CO04_order[i], i + 1);
			PrintFail("CO04", buf);
			ok = false;
			break;
		}
	}
	if (ok && pA == nullptr && pB == nullptr && pC == nullptr)
		PrintPass("CO04", "3개 코루틴 순서 1~9 정확, 모두 nullptr 반환");
	else if (ok)
		PrintFail("CO04", "순서는 맞으나 일부 CoContext가 nullptr이 아님");
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
	PrintSection("TC_CO05: fn()에 전달된 CoContext*가 CoRun 반환값과 동일한지 검증");

	g_CO05_pCtxFromFn = nullptr;
	CoContext* pCtx = CoRun(fn_CO05);

	if (pCtx != nullptr && pCtx == g_CO05_pCtxFromFn)
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoRun=%p == fn_arg=%p", pCtx, g_CO05_pCtxFromFn);
		PrintPass("CO05", buf);
	}
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoRun=%p != fn_arg=%p", pCtx, g_CO05_pCtxFromFn);
		PrintFail("CO05", buf);
	}

	if (pCtx) CoResume(pCtx);
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
	Console::WriteLine(ConsoleColor::Cyan, "    [CO06] 10KB 배열 접근 완료 (arr[0]=%d)", (int)(unsigned char)arr[0]);
	CoYield();
}

static void Test_CO06()
{
	PrintSection("TC_CO06: 10KB 로컬 배열 - VEH 스택 페이지 자동 확장");

	// 기본 설정 복원 (initCount=2, guardCount=3, growCount=2)
	g_cCoMgr.Configure(2, 3, 2);

	CoContext* pCtx = CoRun(fn_CO06);
	if (pCtx != nullptr && pCtx->state_ == csYield)
	{
		// 확장 후 pStackLimit_이 내려갔는지 확인
		CoStack* pStack = &pCtx->stack_;
		Console::WriteLine(ConsoleColor::Cyan,
			"    [CO06] 스택 확장 후: pStackLimit_=0x%p  pStackBase_=0x%p  diff=%lld pages",
			pStack->pStackLimit_, pStack->pStackBase_,
			(long long)(pStack->pStackBase_ - pStack->pStackLimit_) / CO_PAGE_SIZE);
		PrintPass("CO06", "VEH 발동 및 스택 확장 후 yield 성공");
	}
	else
		PrintFail("CO06", "CoRun이 nullptr을 반환하거나 csYield가 아님");

	if (pCtx) CoResume(pCtx);
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
	PrintSection("TC_CO07: Low / Mid / High tier별 코루틴 정상 실행");

	g_cCoMgr.Configure(2, 3, 2);

	const CoStackTier tiers[]   = { cstLow, cstMid, cstHigh };
	const char*       tierNames[] = { "Low", "Mid", "High" };

	for (int i = 0; i < 3; ++i)
	{
		CoContext* pCtx = CoRun(fn_CO07, tiers[i]);
		if (pCtx != nullptr && pCtx->state_ == csYield && pCtx->stack_.stackTier_ == tiers[i])
		{
			char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), "tier=%s pStackBase_=0x%p", tierNames[i], pCtx->stack_.pStackBase_);
			PrintPass("CO07", buf);
			CoResume(pCtx);
		}
		else
		{
			char buf[128];
			StringUtil::FormatBuffer(buf, sizeof(buf), "tier=%s 실패 (pCtx=%p state=%d tier=%d)",
				tierNames[i], pCtx, pCtx ? (int)pCtx->state_ : -1, pCtx ? (int)pCtx->stack_.stackTier_ : -1);
			PrintFail("CO07", buf);
			if (pCtx) g_cCoMgr.FreeCtx(pCtx);
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
	PrintSection("TC_CO08: 로컬 변수 보존 (yield/resume 사이 volatile 지역변수 유지)");

	g_CO08_pass = false;
	CoContext* pCtx = CoRun(fn_CO08);
	CoResume(pCtx);

	if (g_CO08_pass)
		PrintPass("CO08", "volatile 지역변수 a=0xDEAD, b=0xBEEF 유지 확인");
	else
		PrintFail("CO08", "지역변수 값이 변조됨");
}

//--------------------------------------------------------------------------------------
// TC_CO09: 중첩 코루틴 (코루틴 내에서 CoRun/CoResume 호출)
//--------------------------------------------------------------------------------------
static CoContext* g_CO09_innerCtx = nullptr;

static void fn_CO09_inner(CoContext*)
{
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/inner] 시작, yield");
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/inner] resume 후 완료");
}

static void fn_CO09_outer(CoContext*)
{
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/outer] 내부 코루틴 실행");
	g_CO09_innerCtx = CoRun(fn_CO09_inner);
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/outer] 내부 yield 감지, 외부 yield");
	CoYield();
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/outer] resume, 내부 코루틴 재개");
	CoResume(g_CO09_innerCtx);
	Console::WriteLine(ConsoleColor::Cyan, "    [CO09/outer] 완료");
}

static void Test_CO09()
{
	PrintSection("TC_CO09: 중첩 코루틴 (코루틴 내에서 CoRun/CoResume 호출)");

	g_CO09_innerCtx = nullptr;
	CoContext* pOuter = CoRun(fn_CO09_outer);

	// outer가 inner를 실행하고 yield → pOuter=csYield, g_CO09_innerCtx=csYield
	bool innerOk = (g_CO09_innerCtx != nullptr && g_CO09_innerCtx->state_ == csYield);
	bool outerOk = (pOuter != nullptr && pOuter->state_ == csYield);

	if (innerOk && outerOk)
		PrintPass("CO09/first_yield", "outer csYield, inner csYield 확인");
	else
	{
		char buf[192];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"outer=%p(state=%d) inner=%p(state=%d)",
			pOuter, pOuter ? (int)pOuter->state_ : -1,
			g_CO09_innerCtx, g_CO09_innerCtx ? (int)g_CO09_innerCtx->state_ : -1);
		PrintFail("CO09/first_yield", buf);
		if (pOuter) g_cCoMgr.FreeCtx(pOuter);
		return;
	}

	// outer resume → inner resume → inner 완료 → outer 완료 → nullptr
	CoContext* pResult = CoResume(pOuter);
	if (pResult == nullptr)
		PrintPass("CO09/final", "중첩 코루틴 최종 완료, CoResume=nullptr");
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoResume=%p (nullptr 기대)", pResult);
		PrintFail("CO09/final", buf);
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
	PrintSection("TC_CO10: GS StackBase/StackLimit 복원 검증");

	char* mainBase  = (char*)__readgsqword(8);
	char* mainLimit = (char*)__readgsqword(16);

	Console::WriteLine(ConsoleColor::Cyan,
		"    [CO10] 실행 전: gs:[8]=0x%p  gs:[16]=0x%p", mainBase, mainLimit);

	CoContext* pCtx = CoRun(fn_CO10);

	char* curBase  = (char*)__readgsqword(8);
	char* curLimit = (char*)__readgsqword(16);

	if (curBase == mainBase && curLimit == mainLimit)
		PrintPass("CO10/after_run", "CoRun 후 GS 정확히 복원");
	else
	{
		char buf[256];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"gs:[8]=%p(exp %p) gs:[16]=%p(exp %p)", curBase, mainBase, curLimit, mainLimit);
		PrintFail("CO10/after_run", buf);
	}

	if (pCtx) CoResume(pCtx);

	curBase  = (char*)__readgsqword(8);
	curLimit = (char*)__readgsqword(16);

	if (curBase == mainBase && curLimit == mainLimit)
		PrintPass("CO10/after_resume", "CoResume 완료 후 GS 정확히 복원");
	else
	{
		char buf[256];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"gs:[8]=%p(exp %p) gs:[16]=%p(exp %p)", curBase, mainBase, curLimit, mainLimit);
		PrintFail("CO10/after_resume", buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO11: CoResume(nullptr) 안전 처리 → nullptr 반환
//--------------------------------------------------------------------------------------
static void Test_CO11()
{
	PrintSection("TC_CO11: CoResume(nullptr) 안전 처리");

	CoContext* pResult = CoResume(nullptr);
	if (pResult == nullptr)
		PrintPass("CO11", "CoResume(nullptr) = nullptr (안전 처리 확인)");
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf), "CoResume(nullptr) = %p (nullptr 기대)", pResult);
		PrintFail("CO11", buf);
	}
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
	PrintSection("TC_CO12: 동일 fn 5회 반복 CoRun/CoResume (풀링 안정성)");

	g_CO12_count = 0;
	const int N = 5;

	for (int i = 0; i < N; ++i)
	{
		CoContext* pCtx = CoRun(fn_CO12);
		CoResume(pCtx);
	}

	if (g_CO12_count == N * 2)
	{
		char buf[64];
		StringUtil::FormatBuffer(buf, sizeof(buf), "%d회 반복, count=%d", N, g_CO12_count);
		PrintPass("CO12", buf);
	}
	else
	{
		char buf[64];
		StringUtil::FormatBuffer(buf, sizeof(buf), "count=%d (기대 %d)", g_CO12_count, N * 2);
		PrintFail("CO12", buf);
	}
}

//--------------------------------------------------------------------------------------
// TC_CO13: cstCustom tier - 사용자 정의 스택 크기 코루틴
//--------------------------------------------------------------------------------------
static void fn_CO13(CoContext*)
{
	CoYield();
}

static void Test_CO13()
{
	PrintSection("TC_CO13: cstCustom tier - 사용자 정의 스택 크기 (32페이지 = 128KB)");

	const _u32 customSize = 32 * CO_PAGE_SIZE; // 128 KB
	CoContext* pCtx = CoRun(fn_CO13, cstCustom, customSize);

	if (pCtx != nullptr && pCtx->state_ == csYield && pCtx->stack_.stackTier_ == cstCustom)
	{
		_u32 actualSize = pCtx->stack_.size_;
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"cstCustom, size=%u bytes (%u pages)", actualSize, actualSize / CO_PAGE_SIZE);
		PrintPass("CO13", buf);
	}
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"pCtx=%p state=%d tier=%d",
			pCtx, pCtx ? (int)pCtx->state_ : -1, pCtx ? (int)pCtx->stack_.stackTier_ : -1);
		PrintFail("CO13", buf);
		if (pCtx) g_cCoMgr.FreeCtx(pCtx);
		return;
	}

	CoResume(pCtx);
}

//--------------------------------------------------------------------------------------
// TC_CO14: 깊은 재귀 - 스택 페이지 자동 확장 (VEH)
//--------------------------------------------------------------------------------------
static volatile int g_CO14_result = 0;

static int RecursiveSum(int n)
{
	volatile char buf[512]; // 재귀 단계마다 512 bytes 소비
	buf[0] = (char)n;
	if (n <= 0)
		return 0;
	return (int)buf[0] + RecursiveSum(n - 1);
}

static void fn_CO14(CoContext*)
{
	// 약 80단계 재귀, 약 40KB 스택 소비 → VEH 여러 번 발동 예상
	auto pCtx = CoCurrentCtx();
	g_CO14_result = RecursiveSum(80);
	Console::WriteLine(ConsoleColor::Cyan,
		"    [CO14] 재귀 완료: result=%d", g_CO14_result);
	CoYield();
}

static void Test_CO14()
{
	PrintSection("TC_CO14: 깊은 재귀 (VEH 스택 확장 발동)");

	g_cCoMgr.Configure(2, 3, 2);
	g_CO14_result = -1;

	CoContext* pCtx = CoRun(fn_CO14);

	if (pCtx != nullptr && pCtx->state_ == csYield && g_CO14_result >= 0)
	{
		CoStack* pStack = &pCtx->stack_;
		Console::WriteLine(ConsoleColor::Cyan,
			"    [CO14] 확장 후: pStackLimit_=0x%p  diff=%lld pages from base",
			pStack->pStackLimit_,
			(long long)(pStack->pStackBase_ - pStack->pStackLimit_) / CO_PAGE_SIZE);
		PrintPass("CO14", "깊은 재귀 완료, 스택 자동 확장 성공");
	}
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"pCtx=%p state=%d result=%d", pCtx, pCtx ? (int)pCtx->state_ : -1, (int)g_CO14_result);
		PrintFail("CO14", buf);
	}

	if (pCtx) CoResume(pCtx);
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
	PrintSection("TC_CO15: 1000회 yield 스트레스 테스트");

	g_CO15_count = 0;
	CoContext* pCtx = CoRun(fn_CO15);

	int resumeCount = 0;
	while (pCtx != nullptr)
	{
		pCtx = CoResume(pCtx);
		resumeCount++;
	}

	if (g_CO15_count == 1000 && resumeCount == 1000)
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"yield=%d, resume=%d (모두 1000회)", g_CO15_count, resumeCount);
		PrintPass("CO15", buf);
	}
	else
	{
		char buf[128];
		StringUtil::FormatBuffer(buf, sizeof(buf),
			"yield=%d, resume=%d (1000 기대)", g_CO15_count, resumeCount);
		PrintFail("CO15", buf);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_CoYeild()
{
	void* pVeh = AddVectoredExceptionHandler(1, CoVEH);

	// 기본 설정 복원
	g_cCoMgr.Configure(2, 3, 2);

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
	Test_CO14();
	Test_CO15();

	Console::WriteLine(ConsoleColor::Yellow, "\n\n[Test_CoYeild] TC_CO01~TC_CO15 전체 완료");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	g_cCoMgr.Clear();
	RemoveVectoredExceptionHandler(pVeh);
}

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoYeild();

	return 0;
}
