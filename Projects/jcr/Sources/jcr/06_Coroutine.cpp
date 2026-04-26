#include "Core.h"
#include "06_CoroutineImpl1.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 테스트 헬퍼
//////////////////////////////////////////////////////////////////////////////////////////

// 스택 포인터 값 검증
// 반환값: 성공 시 true
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

// 특정 주소의 페이지 상태를 VirtualQuery로 검증
// _expProtectMask: 0이면 Protect 비교 생략
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

// 가드 존 [pGuardLimit_, pStackLimit_) 전체가 PAGE_GUARD | MEM_COMMIT인지,
// pStackLimit_ 바로 위 1페이지가 COMMIT(non-GUARD)인지 검증
// _expGuardCount: 예상 가드 페이지 수
static bool VerifyGuardPages(CoStack* _pStack, _u32 _expGuardCount, const char* _pTag)
{
	// 1. 가드 페이지 수 확인
	_u32 actualGuardCount = (_u32)((_pStack->pStackLimit_ - _pStack->pGuardLimit_) / CO_PAGE_SIZE);
	if (actualGuardCount != _expGuardCount)
	{
		Console::WriteLine(ConsoleColor::Red,
			"  FAIL [%s] guardPageCount: %u  (expected %u)",
			_pTag, actualGuardCount, _expGuardCount);
		return false;
	}

	bool ok = true;

	// 2. 가드 존 내부가 실제로 GUARD 상태인지 확인
	for (_u32 i = 0; i < _expGuardCount; ++i)
	{
		char* pPage = _pStack->pGuardLimit_ + i * CO_PAGE_SIZE;
		char tagBuf[128];
		StringUtil::FormatBuffer(tagBuf, sizeof(tagBuf), "%s/GUARD[%u]", _pTag, i);
		if (!VerifyPageState(pPage, MEM_COMMIT, PAGE_GUARD, tagBuf))
			ok = false;
	}

	// 3. pStackLimit_ 바로 아래에 COMMIT(RW) 페이지가 존재하는지 확인
	//    (가드가 1개 이상인 경우만; 0개면 스택이 바닥까지 닿은 것)
	if (_expGuardCount == 0 && _pStack->pGuardLimit_ == _pStack->pStackLimit_)
	{
		Console::WriteLine(ConsoleColor::Cyan,
			"  INFO [%s] 가드 페이지 0개 - pGuardLimit_ == pStackLimit_ (스택 바닥 도달)", _pTag);
	}

	// 4. pStackLimit_ 위 1페이지는 COMMIT RW
	char* pCommitPage = _pStack->pStackLimit_;
	char tagCommit[128];
	StringUtil::FormatBuffer(tagCommit, sizeof(tagCommit), "%s/COMMIT_above_limit", _pTag);
	// pStackLimit_ 이상은 항상 RW Commit 되어 있어야 한다.
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

// 테스트 섹션 헤더 출력
static void PrintSection(const char* _pName)
{
	Console::WriteLine(ConsoleColor::Yellow,
		"\n================================================================");
	Console::WriteLine(ConsoleColor::Yellow, "  %s", _pName);
	Console::WriteLine(ConsoleColor::Yellow,
		"================================================================");
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC01 ~ TC03: pageGrowCount_ 변화 테스트
//
// 공통 설정: initCount=2, guardCount=3, Mid tier (16 pages)
// 초기 상태:
//   pStackLimit_  = pStackBase_ - 2*PAGE
//   pGuardLimit_  = pStackBase_ - 5*PAGE
//
// 가드 페이지 1회 터치(VEH) 후 예상 변화량:
//   growCount=0 → pStackLimit_ -1*PAGE, pGuardLimit_ -1*PAGE
//   growCount=1 → pStackLimit_ -2*PAGE, pGuardLimit_ -2*PAGE
//   growCount=2 → pStackLimit_ -3*PAGE, pGuardLimit_ -3*PAGE
//////////////////////////////////////////////////////////////////////////////////////////
static void TC_GrowCount(const char* _pName, _u32 _growCount, _u32 _expLimitDelta, _u32 _expGuardDelta)
{
	// growCount에 따라 pStackLimit_이 얼마나 내려가는지 검증
	// _expLimitDelta, _expGuardDelta: 내려갈 페이지 수 (양수)

	g_cCoMgr.Configure(2, 3, _growCount);

	CoContext* pCtx = g_cCoMgr.AllocCtx(cstMid, 0);
	CoStack*   pStack = &pCtx->stack_;

	char* pInitLimit      = pStack->pStackLimit_;
	char* pInitGuardLimit = pStack->pGuardLimit_;

	// 사전 상태 검증
	char tagPre[64];
	StringUtil::FormatBuffer(tagPre, sizeof(tagPre), "%s/pre", _pName);
	VerifyPageState(pInitLimit - CO_PAGE_SIZE, MEM_COMMIT, PAGE_GUARD, tagPre);  // 첫 가드 페이지

	// 가드 페이지 터치 → VEH 발동 → ExpandStack 호출
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;

	// 사후 검증
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
// TC04 ~ TC07: 가드 페이지 설치 가능 수 소진 테스트
//
// 설정: initCount=60, guardCount=3, growCount=0, High tier (64 pages)
//
// 초기 상태 (High = 64 pages):
//   pStackBase_   = pStackEnd_ + 64*PAGE
//   pStackLimit_  = pStackBase_ - 60*PAGE = pStackEnd_ + 4*PAGE
//   pGuardLimit_  = pStackBase_ - 63*PAGE = pStackEnd_ + 1*PAGE
//   예약(RESERVE) = pStackEnd_ ~ pStackEnd_+1*PAGE (1페이지)
//
// 순차적 터치에 따른 상태 변화:
//   touch 1(TC04) → pStackLimit_=+3*PAGE, pGuardLimit_=+0*PAGE  → 가드 3개 (pStackEnd_ 기준)
//   touch 2(TC05) → pStackLimit_=+2*PAGE, pGuardLimit_=+0*PAGE  → 가드 2개
//   touch 3(TC06) → pStackLimit_=+1*PAGE, pGuardLimit_=+0*PAGE  → 가드 1개
//   touch 4(TC07) → pStackLimit_=+0*PAGE, pGuardLimit_=+0*PAGE  → 가드 0개
//////////////////////////////////////////////////////////////////////////////////////////
static void TC04_to_TC07_GuardExhaustion()
{
	PrintSection("TC04~TC07: 가드 페이지 설치 가능 수 소진 테스트 (growCount=0, High tier 64-pages)");

	// initCount=60 → 64-page High 스택에서 60페이지를 RW commit, 3페이지 guard, 1페이지 reserve
	g_cCoMgr.Configure(60, 3, 0);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstHigh, 0);
	CoStack*   pStack = &pCtx->stack_;

	char* pEnd = pStack->pStackEnd_;

	// --- 사전 상태 검증 ---
	Console::WriteLine(ConsoleColor::Cyan, "\n[초기 상태]");
	VerifyPtrs(pStack, pEnd + 4 * CO_PAGE_SIZE, pEnd + 1 * CO_PAGE_SIZE, "TC04~07/init");
	// 맨 아래 1페이지: RESERVE
	VerifyPageState(pEnd, MEM_RESERVE, 0, "TC04~07/init/bottom_reserve");
	// 가드 존: [pEnd+1*PAGE, pEnd+4*PAGE) — 3페이지 GUARD
	VerifyGuardPages(pStack, 3, "TC04~07/init");

	//----------------------------------------------------------------------
	// TC04: 1번째 터치 → 가드 3개 설치 가능 (정상 풀 설치)
	//   pFaultStart = pEnd+3*PAGE
	//   pNewGuardStart = pEnd+3*PAGE - 3*PAGE = pEnd → clamped to pEnd (pEnd <= pEnd)
	//   newGuardCount = (pEnd+3*PAGE - pEnd)/PAGE = 3
	//----------------------------------------------------------------------
	PrintSection("TC04: 1번째 터치 - 가드 3개 설치 (정상)");
	Console::WriteLine(ConsoleColor::White, "  touch addr: pStackLimit_-4 = 0x%p", pStack->pStackLimit_ - 4);

	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;   // VEH 발동

	VerifyPtrs(pStack, pEnd + 3 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC04/ptrs");
	// 가드 존: [pEnd, pEnd+3*PAGE) — 3페이지 GUARD
	VerifyGuardPages(pStack, 3, "TC04/guard");
	// pEnd+3*PAGE(=new pStackLimit_) 위 1페이지는 COMMIT RW
	VerifyPageState(pEnd + 3 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC04/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC04 After");

	//----------------------------------------------------------------------
	// TC05: 2번째 터치 → 가드 2개만 설치 가능
	//   pFaultStart = pEnd+2*PAGE
	//   pNewGuardStart = pEnd+2*PAGE - 3*PAGE = pEnd-PAGE → clamped to pEnd
	//   newGuardCount = (pEnd+2*PAGE - pEnd)/PAGE = 2
	//----------------------------------------------------------------------
	PrintSection("TC05: 2번째 터치 - 가드 2개만 설치 가능");
	Console::WriteLine(ConsoleColor::White, "  touch addr: pStackLimit_-4 = 0x%p", pStack->pStackLimit_ - 4);

	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;   // VEH 발동

	VerifyPtrs(pStack, pEnd + 2 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC05/ptrs");
	// 가드 존: [pEnd, pEnd+2*PAGE) — 2페이지 GUARD
	VerifyGuardPages(pStack, 2, "TC05/guard");
	// pEnd+2*PAGE(=new pStackLimit_) 위 1페이지는 COMMIT RW
	VerifyPageState(pEnd + 2 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC05/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC05 After");

	//----------------------------------------------------------------------
	// TC06: 3번째 터치 → 가드 1개만 설치 가능
	//   pFaultStart = pEnd+1*PAGE
	//   pNewGuardStart = pEnd+1*PAGE - 3*PAGE = pEnd-2*PAGE → clamped to pEnd
	//   newGuardCount = (pEnd+1*PAGE - pEnd)/PAGE = 1
	//----------------------------------------------------------------------
	PrintSection("TC06: 3번째 터치 - 가드 1개만 설치 가능");
	Console::WriteLine(ConsoleColor::White, "  touch addr: pStackLimit_-4 = 0x%p", pStack->pStackLimit_ - 4);

	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;   // VEH 발동

	VerifyPtrs(pStack, pEnd + 1 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC06/ptrs");
	// 가드 존: [pEnd, pEnd+1*PAGE) — 1페이지 GUARD
	VerifyGuardPages(pStack, 1, "TC06/guard");
	// pEnd+1*PAGE(=new pStackLimit_) 위 1페이지는 COMMIT RW
	VerifyPageState(pEnd + 1 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC06/commit_above_limit");
	g_cCoMgr.DumpStack(pStack, "TC06 After");

	//----------------------------------------------------------------------
	// TC07: 4번째 터치 → 가드 0개 (스택 바닥 도달, 더 이상 가드 설치 불가)
	//   pFaultStart = pEnd
	//   pNewGuardStart = pEnd - 3*PAGE → clamped to pEnd
	//   newGuardCount = (pEnd - pEnd)/PAGE = 0
	//   pGuardLimit_ == pStackLimit_ == pStackEnd_ → 가드 없음
	//----------------------------------------------------------------------
	PrintSection("TC07: 4번째 터치 - 가드 0개 (바닥 도달)");
	Console::WriteLine(ConsoleColor::White, "  touch addr: pStackLimit_-4 = 0x%p", pStack->pStackLimit_ - 4);

	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;   // VEH 발동

	VerifyPtrs(pStack, pEnd + 0 * CO_PAGE_SIZE, pEnd + 0 * CO_PAGE_SIZE, "TC07/ptrs");
	// 가드 페이지 0개: pGuardLimit_ == pStackLimit_ == pStackEnd_
	VerifyGuardPages(pStack, 0, "TC07/guard");
	// pEnd(=pStackLimit_) 위 1페이지는 COMMIT RW
	VerifyPageState(pEnd + 0 * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE, "TC07/commit_at_limit");
	g_cCoMgr.DumpStack(pStack, "TC07 After");

	g_cCoMgr.FreeCtx(pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_CoStack()
{
	void* pVeh = AddVectoredExceptionHandler(1, CoVEH);

	//----------------------------------------------------------------------
	// TC01: pageGrowCount_ = 0
	//   폴트 페이지만 해제, 추가 grow 없음
	//   pStackLimit_ -1 PAGE, pGuardLimit_ -1 PAGE
	//----------------------------------------------------------------------
	PrintSection("TC01: pageGrowCount_=0 (추가 grow 없음)");
	TC_GrowCount("TC01", 0, 1, 1);

	//----------------------------------------------------------------------
	// TC02: pageGrowCount_ = 1
	//   폴트 페이지 + 1 추가 grow
	//   pStackLimit_ -2 PAGE, pGuardLimit_ -2 PAGE
	//----------------------------------------------------------------------
	PrintSection("TC02: pageGrowCount_=1 (1 페이지 추가 grow)");
	TC_GrowCount("TC02", 1, 2, 2);

	//----------------------------------------------------------------------
	// TC03: pageGrowCount_ = 2 (기본값)
	//   폴트 페이지 + 2 추가 grow
	//   pStackLimit_ -3 PAGE, pGuardLimit_ -3 PAGE
	//----------------------------------------------------------------------
	PrintSection("TC03: pageGrowCount_=2 (2 페이지 추가 grow, 기본값)");
	TC_GrowCount("TC03", 2, 3, 3);

	//----------------------------------------------------------------------
	// TC04 ~ TC07: 가드 페이지 설치 가능 수 소진 (3→2→1→0)
	//----------------------------------------------------------------------
	TC04_to_TC07_GuardExhaustion();

	Console::WriteLine(ConsoleColor::Yellow, "\n\n[Test_CoRun] 전체 테스트 완료");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	g_cCoMgr.Clear();
	jc::SingletonPointer<CoMgr>::Free();
	RemoveVectoredExceptionHandler(pVeh);
}

//////////////////////////////////////////////////////////////////////////////////////////
void test_func(CoContext* _pContext)
{
	int a = 30;

	Console::WriteLine(ConsoleColor::Green, "Coroutine function 1 yield. Context: %p", _pContext);
	CoYield();
	Console::WriteLine(ConsoleColor::Green, "Coroutine function 1 resume and finished. Context: %p", _pContext);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Test_CoYeild()
{
	void* pVeh = AddVectoredExceptionHandler(1, CoVEH);

	CoContext* pCtx = CoRun(test_func);

	CoResume(pCtx);

	RemoveVectoredExceptionHandler(pVeh);
}

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoYeild();
	/*auto pMenu = jc::MakeShared<ConsoleMenuItem>();
	pMenu->AddHeader(
		"========================================\n"
		"         Coroutine R&D - 코루틴 실전 연구\n"
		"========================================");
	pMenu->Add("1", "CoroutineImpl1 - 코루틴 구현 1", CONSOLE_MENU_ACTION(Test_CoRun()));
	pMenu->AddBack("0", "뒤로가기");

	ConsoleMenuItem::Show(pMenu);*/

	return 0;
}
