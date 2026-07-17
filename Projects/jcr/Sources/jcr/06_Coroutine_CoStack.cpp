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
	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(_growCount);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstMid, 0);
	CoStack*   pStack = &pCtx->stack_;

	g_cCoMgr.DumpStack(pStack);
	g_cCoMgr.currentCtx_ = pCtx;

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
	g_cCoMgr.currentCtx_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_StackGrowth: 가드 페이지 소진 테스트 (TC04~TC07)
//////////////////////////////////////////////////////////////////////////////////////////
static void TC04_to_TC07_GuardExhaustion()
{
	PrintSection("TC04~TC07: 가드 페이지 설치 가능 수 소진 테스트 (growCount=0, High tier 64-pages)");

	g_cCoMgr.SetPageInitCount(CO_STACK_PAGE_COUNT_LOW - 2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(0);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstLow, 0);
	CoStack*   pStack = &pCtx->stack_;

	// 강제로 설정
	g_cCoMgr.currentCtx_ = pCtx;
	char* pEnd = pStack->pStackEnd_;

	g_cCoMgr.DumpStack(pStack, "TC04 초기 상태");

	PrintSection("TC04: 오버플로 페이지 가드 터치");

	_u32 code = 0;
	// *(volatile char*)(pStack->pStackLimit_ - 4) = 0; // 스택 오버플로 발생함.

	g_cCoMgr.FreeCtx(pCtx);
	g_cCoMgr.currentCtx_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_InitStack_AlwaysLeavesGuard: InitStack이 마지막에 가드 페이지를 남기는지 테스트
//////////////////////////////////////////////////////////////////////////////////////////
static void TC_InitStack_AlwaysLeavesGuard()
{
	PrintSection("TC08: InitStack - 가드 페이지 설치 확인");

	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(2);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstMid, 0);
	CoStack*   pStack = &pCtx->stack_;

	g_cCoMgr.currentCtx_ = pCtx;

	Console::WriteLine(ConsoleColor::Cyan, "\n[InitStack 후 상태]");
	Console::WriteLine(ConsoleColor::Cyan, "  pStackBase_  = 0x%p", pStack->pStackBase_);
	Console::WriteLine(ConsoleColor::Cyan, "  pStackLimit_ = 0x%p", pStack->pStackLimit_);
	Console::WriteLine(ConsoleColor::Cyan, "  pGuardLimit_ = 0x%p", pStack->pGuardLimit_);
	Console::WriteLine(ConsoleColor::Cyan, "  pStackEnd_   = 0x%p", pStack->pStackEnd_);

	// InitStack 후에는 항상 페이지 가드가 있어야 함
	_u32 guardPageCount = (_u32)((pStack->pStackLimit_ - pStack->pGuardLimit_) / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Cyan, "  Guard page count = %u", guardPageCount);

	bool hasGuard = (pStack->pGuardLimit_ < pStack->pStackLimit_);
	if (hasGuard)
	{
		PrintPass("TC08/init_guard", "InitStack 후 가드 페이지 존재");
		VerifyGuardPages(pStack, 3, "TC08/init");
	}
	else
	{
		PrintFail("TC08/init_guard", "InitStack 후 가드 페이지 없음");
	}

	// pGuardLimit_이 pStackEnd_ + CO_PAGE_SIZE (오버플로우 가드) 이상이어야 함
	char* pOverflowGuardTop = pStack->pStackEnd_ + CO_PAGE_SIZE;
	if (pStack->pGuardLimit_ >= pOverflowGuardTop)
	{
		PrintPass("TC08/overflow_guard", "오버플로우 가드 페이지 유지됨");
	}
	else
	{
		PrintFail("TC08/overflow_guard", "오버플로우 가드 페이지 침범됨");
	}

	// Commit 영역 바로 아래가 가드 페이지 시작이어야 함
	char* pFirstGuardPage = pStack->pStackLimit_;
	MEMORY_BASIC_INFORMATION mbi = {};
	VirtualQuery(pFirstGuardPage, &mbi, sizeof(mbi));
	if ((mbi.Protect & PAGE_GUARD) != 0)
	{
		PrintPass("TC08/first_guard", "가드 페이지의 첫 페이지 올바름");
	}
	else
	{
		PrintFail("TC08/first_guard", "가드 페이지의 첫 페이지가 PAGE_GUARD 아님");
	}

	g_cCoMgr.DumpStack(pStack, "TC08 After InitStack");
	g_cCoMgr.FreeCtx(pCtx);
	g_cCoMgr.currentCtx_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_ExpandStack_AlwaysLeavesGuard: ExpandStack이 마지막에 가드 페이지를 남기는지 테스트
//////////////////////////////////////////////////////////////////////////////////////////
static void TC_ExpandStack_AlwaysLeavesGuard()
{
	PrintSection("TC09: ExpandStack - 가드 페이지 설치 확인");

	g_cCoMgr.SetPageInitCount(2);
	g_cCoMgr.SetPageGuardCount(3);
	g_cCoMgr.SetPageGrowCount(1);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstHigh, 0);
	CoStack*   pStack = &pCtx->stack_;
	g_cCoMgr.currentCtx_ = pCtx;

	Console::WriteLine(ConsoleColor::Cyan, "\n[ExpandStack 전 상태]");
	Console::WriteLine(ConsoleColor::Cyan, "  pStackBase_  = 0x%p", pStack->pStackBase_);
	Console::WriteLine(ConsoleColor::Cyan, "  pStackLimit_ = 0x%p", pStack->pStackLimit_);
	Console::WriteLine(ConsoleColor::Cyan, "  pGuardLimit_ = 0x%p", pStack->pGuardLimit_);

	_u32 guardBefore = (_u32)((pStack->pStackLimit_ - pStack->pGuardLimit_) / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Cyan, "  Guard pages before = %u", guardBefore);

	// ExpandStack 트리거 (가드 페이지 터치)
	*(volatile char*)(pStack->pStackLimit_ - 4) = 0;

	Console::WriteLine(ConsoleColor::Cyan, "\n[ExpandStack 후 상태]");
	Console::WriteLine(ConsoleColor::Cyan, "  pStackBase_  = 0x%p", pStack->pStackBase_);
	Console::WriteLine(ConsoleColor::Cyan, "  pStackLimit_ = 0x%p", pStack->pStackLimit_);
	Console::WriteLine(ConsoleColor::Cyan, "  pGuardLimit_ = 0x%p", pStack->pGuardLimit_);

	_u32 guardAfter = (_u32)((pStack->pStackLimit_ - pStack->pGuardLimit_) / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Cyan, "  Guard pages after = %u", guardAfter);

	// ExpandStack 후에도 항상 페이지 가드가 있어야 함
	bool hasGuard = (pStack->pGuardLimit_ < pStack->pStackLimit_);
	if (hasGuard)
	{
		PrintPass("TC09/expand_guard", "ExpandStack 후 가드 페이지 존재");
		VerifyGuardPages(pStack, 3, "TC09/expand");
	}
	else
	{
		PrintFail("TC09/expand_guard", "ExpandStack 후 가드 페이지 없음");
	}

	// pGuardLimit_이 pStackEnd_ + CO_PAGE_SIZE (오버플로우 가드) 이상이어야 함
	char* pOverflowGuardTop = pStack->pStackEnd_ + CO_PAGE_SIZE;
	if (pStack->pGuardLimit_ >= pOverflowGuardTop)
	{
		PrintPass("TC09/overflow_guard", "오버플로우 가드 페이지 유지됨");
	}
	else
	{
		PrintFail("TC09/overflow_guard", "오버플로우 가드 페이지 침범됨");
	}

	// 새로운 pStackLimit_이 이전 pGuardLimit_보다 아래에 있어야 함
	if (pStack->pStackLimit_ < pStack->pStackBase_ - 2 * CO_PAGE_SIZE)
	{
		PrintPass("TC09/limit_moved", "pStackLimit_ 확장됨");
	}
	else
	{
		PrintFail("TC09/limit_moved", "pStackLimit_ 확장 안됨");
	}

	// 마지막 페이지 가드는 PAGE_GUARD 설정되어야 함
	char* pLastGuardPage = pStack->pGuardLimit_;
	MEMORY_BASIC_INFORMATION mbi = {};
	VirtualQuery(pLastGuardPage, &mbi, sizeof(mbi));
	if ((mbi.Protect & PAGE_GUARD) != 0)
	{
		PrintPass("TC09/last_guard", "가드 페이지의 마지막 페이지 올바름");
	}
	else
	{
		PrintFail("TC09/last_guard", "가드 페이지의 마지막 페이지가 PAGE_GUARD 아님");
	}

	g_cCoMgr.DumpStack(pStack, "TC09 After ExpandStack");
	g_cCoMgr.FreeCtx(pCtx);
	g_cCoMgr.currentCtx_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TC_MultipleExpansions_AlwaysLeavesGuard: 여러 번 확장해도 가드 페이지가 항상 남는지 테스트
//////////////////////////////////////////////////////////////////////////////////////////
static void TC_MultipleExpansions_AlwaysLeavesGuard()
{
	PrintSection("TC10: 다중 확장 - 매번 가드 페이지 설치 확인");

	g_cCoMgr.SetPageInitCount(3);
	g_cCoMgr.SetPageGuardCount(2);
	g_cCoMgr.SetPageGrowCount(0);

	CoContext* pCtx  = g_cCoMgr.AllocCtx(cstHigh, 0);
	CoStack*   pStack = &pCtx->stack_;

	g_cCoMgr.currentCtx_ = pCtx;
	char* pOverflowGuardTop = pStack->pStackEnd_ + CO_PAGE_SIZE;

	for (int expansionCount = 0; expansionCount < 3; ++expansionCount)
	{
		Console::WriteLine(ConsoleColor::Cyan, "\n[확장 %d]", expansionCount + 1);

		// 확장 전 상태
		_u32 guardBefore = (_u32)((pStack->pStackLimit_ - pStack->pGuardLimit_) / CO_PAGE_SIZE);
		Console::WriteLine(ConsoleColor::Cyan, "  Before: pStackLimit_=0x%p, pGuardLimit_=0x%p, guards=%u",
			pStack->pStackLimit_, pStack->pGuardLimit_, guardBefore);

		// 스택 확장 트리거
		*(volatile char*)(pStack->pStackLimit_ - 4) = 0;

		// 확장 후 상태
		_u32 guardAfter = (_u32)((pStack->pStackLimit_ - pStack->pGuardLimit_) / CO_PAGE_SIZE);
		Console::WriteLine(ConsoleColor::Cyan, "  After: pStackLimit_=0x%p, pGuardLimit_=0x%p, guards=%u",
			pStack->pStackLimit_, pStack->pGuardLimit_, guardAfter);

		// 검증
		bool hasGuard = (pStack->pGuardLimit_ < pStack->pStackLimit_);
		if (hasGuard && pStack->pGuardLimit_ >= pOverflowGuardTop)
		{
			char tagBuf[64];
			StringUtil::FormatBuffer(tagBuf, sizeof(tagBuf), "TC10/exp%d", expansionCount + 1);
			PrintPass(tagBuf, "가드 페이지 유지됨");
			VerifyGuardPages(pStack, 2, tagBuf);
		}
		else
		{
			char tagBuf[64];
			StringUtil::FormatBuffer(tagBuf, sizeof(tagBuf), "TC10/exp%d", expansionCount + 1);
			if (!hasGuard)
				PrintFail(tagBuf, "가드 페이지 없음");
			else
				PrintFail(tagBuf, "오버플로우 가드 페이지 침범됨");
		}
	}

	g_cCoMgr.DumpStack(pStack, "TC10 After Multiple Expansions");
	g_cCoMgr.FreeCtx(pCtx);
	g_cCoMgr.currentCtx_ = nullptr;
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

	TC_InitStack_AlwaysLeavesGuard();

	TC_ExpandStack_AlwaysLeavesGuard();

	TC_MultipleExpansions_AlwaysLeavesGuard();

	Console::WriteLine(ConsoleColor::Yellow, "\n\n[Test_CoStack] 전체 테스트 완료");
	Console::ReadKeyWhile("Press spacebar to continue...", ConsoleKey::Spacebar);

	g_cCoMgr.Clear();
	RemoveVectoredExceptionHandler(pVeh);
}
