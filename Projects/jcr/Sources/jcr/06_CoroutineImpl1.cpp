#include "Core.h"
#include "06_CoroutineImpl1.h"

#define CO_ERROR_NULL_FUNCTION	1
#define CO_ERROR_VIRTUAL_ALLOC	2

_u32 g_coNextId_    = 0;
_u32 g_cCoLastError = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// Configure (테스트 전용)
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::Configure(_u32 _pageInitCount, _u32 _pageGuardCount, _u32 _pageGrowCount)
{
	pageInitCount_  = _pageInitCount;
	pageGuardCount_ = _pageGuardCount;
	pageGrowCount_  = _pageGrowCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] InitStack
//   CoStack의 pStackEnd_, size_, stackTier_ 가 설정된 상태에서 호출.
//   스택 상단 pageInitCount_ 페이지 commit + pageGuardCount_ 페이지 PAGE_GUARD commit.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::InitStack(CoStack* _pStack)
{
	_pStack->pStackBase_ = _pStack->pStackEnd_ + _pStack->size_;

	//  pStackBase_  ──────────────── (높은 주소, 초기 RSP)
	//               | init pages  |  ← pageInitCount_ 개 commit
	//  pStackLimit_ ────────────────
	//               | guard pages |  ← pageGuardCount_ 개 commit + PAGE_GUARD
	//               ────────────────
	//               |  (reserve)  |
	//  pStackEnd_   ──────────────── (낮은 주소)

	char* pCommitAddr = _pStack->pStackBase_ - (pageInitCount_ * CO_PAGE_SIZE);
	if (VirtualAlloc(pCommitAddr, pageInitCount_ * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE) == nullptr)
	{
		_LogError_("VirtualAlloc (init commit) failed. Error: %lu", GetLastError());
		return;
	}

	char* pGuardAddr = pCommitAddr - (pageGuardCount_ * CO_PAGE_SIZE);
	if (VirtualAlloc(pGuardAddr, pageGuardCount_ * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
	{
		_LogError_("VirtualAlloc (guard commit) failed. Error: %lu", GetLastError());
		return;
	}

	_pStack->pStackLimit_ = pCommitAddr;
	_pStack->pGuardLimit_ = pGuardAddr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] AllocStack
//   tier/size에 따라 메모리를 예약(RESERVE)하고 InitStack을 호출.
//   풀에 재사용 가능한 CoStack이 있으면 먼저 꺼내서 사용.
//   _pStack에 결과를 복사하여 반환.
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::AllocStack(OUT CoStack* _pStack, CoStackTier _stackTier, _u32 _stackSize)
{
	if (_pStack == nullptr)
	{
		jc_assert_msg(false, "스택 구조체 포인터가 nullptr입니다.");
		return false;
	}

	if (_stackTier <= 0 || _stackTier > cstValidTierEnd)
	{
		jc_assert_msg(false, "잘못된 스택 티어입니다. tier: %d", _stackTier);
		return false;
	}

	// 티어 → 크기 결정
	_u32 stackSize = _stackSize;
	switch (_stackTier)
	{
	case cstLow:    stackSize = CO_STACK_SIZE_LOW;  break;
	case cstMid:    stackSize = CO_STACK_SIZE_MID;  break;
	case cstHigh:   stackSize = CO_STACK_SIZE_HIGH; break;
	default: break;  // cstCustom: _stackSize 그대로 사용
	}

	char* pBase = (char*)VirtualAlloc(nullptr, stackSize, MEM_RESERVE, PAGE_NOACCESS);
	if (pBase == nullptr)
	{
		g_cCoLastError = CO_ERROR_VIRTUAL_ALLOC;
		return false;
	}

	_pStack->pStackEnd_  = pBase;
	_pStack->size_       = stackSize;
	_pStack->stackTier_  = _stackTier;
	InitStack(_pStack);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] FreeStack
//   풀 티어: decommit (예약 유지 → 재사용 가능)
//   커스텀:  MEM_RELEASE (완전 해제)
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::FreeStack(CoStack* _pStack)
{
	if (_pStack->stackTier_ == cstCustom)
	{
		VirtualFree(_pStack->pBaseAddr_, 0, MEM_RELEASE);
	}
	else
	{
		VirtualFree(_pStack->pBaseAddr_, _pStack->size_, MEM_DECOMMIT);
	}
	_pStack->pStackLimit_ = nullptr;
	_pStack->pGuardLimit_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// InitCtx
//   풀에서 꺼낸 CoContext를 재초기화. 스택은 decommit 상태이므로 InitStack으로 재commit.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::InitCtx(CoContext* _pCtx)
{
	InitStack(&_pCtx->stack_);
	_pCtx->id_       = 0;
	_pCtx->threadId_ = 0;
	_pCtx->regs_     = {};
	_pCtx->state_    = csInit;
	_pCtx->fn_       = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// AllocCtx
//   풀에 재사용 가능한 CoContext가 있으면 꺼내서 InitCtx 후 반환.
//   없으면 새로 dbg_new 하고 AllocStack → using_ 등록 후 반환.
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::AllocCtx(CoStackTier _stackTier, _u32 _stackSize)
{
	if (_stackTier <= 0 || _stackTier > cstValidTierEnd)
	{
		jc_assert_msg(false, "잘못된 스택 티어입니다. tier: %d", _stackTier);
		return nullptr;
	}

	// 커스텀 크기 → 실제 티어 판별 (AllocStack과 동일 로직)
	CoStackTier resolvedTier = _stackTier;
	if (_stackTier == cstCustom)
	{
		if (_stackSize < CO_STACK_SIZE_LOW)        resolvedTier = cstLow;
		else if (_stackSize < CO_STACK_SIZE_MID)   resolvedTier = cstMid;
		else if (_stackSize < CO_STACK_SIZE_HIGH)  resolvedTier = cstHigh;
		// else: 진짜 Custom, 풀링 없음
	}

	CoContext* pCtx = nullptr;

	if (resolvedTier != cstCustom && free_[resolvedTier].PopFront(&pCtx))
	{
		// 풀에서 재사용: decommit된 상태 → InitStack이 다시 commit
		InitCtx(pCtx);
	}
	else
	{
		pCtx = dbg_new CoContext();
		if (!AllocStack(&pCtx->stack_, _stackTier, _stackSize))
		{
			delete pCtx;
			return nullptr;
		}
		pCtx->state_ = csInit;
	}

	pCtx->id_ = ++nextId_;
	using_.Insert(pCtx->stack_.pStackBase_, pCtx);
	return pCtx;
}

//////////////////////////////////////////////////////////////////////////////////////////
// FreeCtx
//   using_ 에서 제거 후 커스텀이면 메모리 해제 + delete,
//   풀 티어면 decommit 후 free_ 풀로 반환.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::FreeCtx(CoContext* _pCtx)
{
	jc_assert(_pCtx != nullptr);

	CoStackTier stackTier = _pCtx->stack_.stackTier_;
	jc_assert_msg(stackTier >= cstValidTierBegin && stackTier <= cstValidTierEnd,
		"잘못된 스택 티어입니다. tier: %d", stackTier);

	CoContext* pPopped = nullptr;
	if (using_.TryPop(_pCtx->stack_.pStackBase_, &pPopped) == false)
	{
		jc_assert_msg(false, "해당 컨텍스트는 관리 중인 컨텍스트가 아닙니다. pStackBase_: 0x%p",
			_pCtx->stack_.pStackBase_);
		return;
	}

	FreeStack(&pPopped->stack_);

	if (stackTier == cstCustom)
	{
		delete pPopped;
	}
	else
	{
		free_[stackTier].PushBack(pPopped);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Clear
//   using_ 에 남은 항목 경고 후 free_ 풀 전체 해제.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::Clear()
{
	if (using_.Size() > 0)
	{
		jc_assert_msg(false, "Clear 호출 시점에 아직 할당된 컨텍스트가 존재합니다. Count: %zu", using_.Size());
	}

	for (int tier = cstReservedTierBegin; tier <= cstReservedTierEnd; ++tier)
	{
		CoContext* pCtx = nullptr;
		while (free_[tier].PopFront(&pCtx))
		{
			VirtualFree(pCtx->stack_.pStackEnd_, 0, MEM_RELEASE);
			delete pCtx;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// FindContextByBase / FindContextByAddr
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::FindContextByBase(char* _pBase)
{
	CoContext** pFound = using_.Find(_pBase);
	if (pFound == nullptr)
		return nullptr;
	return *pFound;
}

CoContext* CoMgr::FindContextByAddr(char* _pAddr)
{
	CoContext** pFound = using_.LowerBoundValue(_pAddr);
	if (pFound == nullptr)
		return nullptr;

	CoStack& stack = (*pFound)->stack_;
	if (_pAddr < stack.pStackEnd_ || _pAddr >= stack.pStackBase_)
		return nullptr;
	return *pFound;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryFindContextByBase / TryFindContextByAddr
//   assert 없이 nullptr 시 false 반환.
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::TryFindContextByBase(char* _pBase, OUT CoContext** _pOut)
{
	CoContext** pFound = using_.Find(_pBase);
	if (pFound == nullptr)
		return false;
	if (_pOut) *_pOut = *pFound;
	return true;
}

bool CoMgr::TryFindContextByAddr(char* _pAddr, OUT CoContext** _pOut)
{
	CoContext* pCtx = FindContextByAddr(_pAddr);
	if (pCtx == nullptr)
		return false;
	if (_pOut) *_pOut = pCtx;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// FindStackByBase / FindStackByAddr
//   CoContext 내부의 CoStack* 을 반환 (ExpandStack, DumpStack 등에서 사용).
//////////////////////////////////////////////////////////////////////////////////////////
CoStack* CoMgr::FindStackByBase(char* _pBase)
{
	CoContext* pCtx = FindContextByBase(_pBase);
	if (pCtx == nullptr)
		return nullptr;
	return &pCtx->stack_;
}

CoStack* CoMgr::FindStackByAddr(char* _pAddr)
{
	CoContext* pCtx = FindContextByAddr(_pAddr);
	if (pCtx == nullptr)
		return nullptr;
	return &pCtx->stack_;
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryFindStackByBase / TryFindStackByAddr
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::TryFindStackByBase(char* _pBase, OUT CoStack** _pOut)
{
	CoStack* pStack = FindStackByBase(_pBase);
	if (pStack == nullptr)
		return false;
	if (_pOut) *_pOut = pStack;
	return true;
}

bool CoMgr::TryFindStackByAddr(char* _pAddr, OUT CoStack** _pOut)
{
	CoStack* pStack = FindStackByAddr(_pAddr);
	if (pStack == nullptr)
		return false;
	if (_pOut) *_pOut = pStack;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// ExpandStack
// VEH에서 호출: 가드 페이지 터치 시 스택 확장 처리
// 처리 흐름:
//   [Before]                              [After]
//   pStackLimit_                           pStackLimit_ (old)
//    GUARD page 0  ← _pFaultAddr            RW page 0       ← fault 페이지 (OS가 GUARD 해제)
//    GUARD page 1                           RW grow 0       ← pageGrowCount_ 개 GUARD 해제
//    GUARD page 2                           RW grow 1          (명시적 VirtualProtect)
//   pGuardLimit_                         pStackLimit_ (new) = old pGuardLimit_
//    (reserved)                             NEW GUARD 0      ← pageGuardCount_ 페이지 새로
//    (reserved)                             NEW GUARD 1         commit + PAGE_GUARD 설정
//    (reserved)                             NEW GUARD 2
//   pStackEnd_                           pGuardLimit_ (new) = old pGuardLimit_ - pageGuardCount_ * PAGE
//                                          (reserved)
//                                         pStackEnd_
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::ExpandStack(CoStack* _pStack, char* _pFaultAddr)
{
	if (pageGuardCount_ == 0)
	{
		// 페이지 가드 갯수가 없는 경우는 존재할 수 없다.
		// 말도안되는 경우 이므로 크래쉬.
		return false;
	}

	// 여기서 Start는 낮은 주소를 나타냄
	// End는 높은 주소의 끝부분을 나타냄
	//
	// ------------------- End		0x85000
	// |
	// |
	// ------------------- Start	0x80000

	// fault 난 페이지의 시작 주소 (페이지 정렬)
	char* pFaultStart = (char*)((uintptr_t)_pFaultAddr & ~(CO_PAGE_SIZE - 1));
	char* pFaultEnd   = pFaultStart + pageGuardCount_ * CO_PAGE_SIZE;

	if (pFaultEnd >= _pStack->pStackBase_)
		pFaultEnd = _pStack->pStackBase_;

	_u32  commitPageCount = (_u32)((pFaultEnd - pFaultStart) / CO_PAGE_SIZE);
	char* pGrowthStart    = pFaultStart;
	char* pGrowthEnd      = pFaultStart;

	if (pageGrowCount_ > 0)
	{
		pGrowthStart = pGrowthStart - pageGrowCount_ * CO_PAGE_SIZE;

		if (pGrowthStart <= _pStack->pStackEnd_)
			pGrowthStart = _pStack->pStackEnd_;

		commitPageCount += (_u32)((pGrowthEnd - pGrowthStart) / CO_PAGE_SIZE);
	}

	if (VirtualAlloc(pGrowthStart, CO_PAGE_SIZE * commitPageCount, MEM_COMMIT, PAGE_READWRITE) == nullptr)
		return false;

	_pStack->pStackLimit_  = pGrowthStart;
	char* pNewGuardEnd     = pGrowthStart;
	char* pNewGuardStart   = pGrowthStart - pageGuardCount_ * CO_PAGE_SIZE;

	if (pNewGuardStart <= _pStack->pStackEnd_)
		pNewGuardStart = _pStack->pStackEnd_;

	_u32 newGuardPageCount = (_u32)((pNewGuardEnd - pNewGuardStart) / CO_PAGE_SIZE);
	if (newGuardPageCount > 0)
	{
		if (VirtualAlloc(pNewGuardStart, CO_PAGE_SIZE * newGuardPageCount, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
		{
			// 페이지 가드 설정 실패는 치명적이므로 크래쉬
			return false;
		}
	}
	_pStack->pGuardLimit_ = pNewGuardStart;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// DumpStack
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::DumpStack(CoStack* _pStack, const char* _pTitle /*= nullptr*/)
{
	if (_pTitle)
		Console::WriteLine(ConsoleColor::Green, "\n==== %s ====", _pTitle);

	// ── CoStack 멤버 ──────────────────────────────────────────────────────────
	const char* pTierName = "Unknown";
	switch (_pStack->stackTier_)
	{
	case cstNone:   pTierName = "None";   break;
	case cstLow:    pTierName = "Low";    break;
	case cstMid:    pTierName = "Mid";    break;
	case cstHigh:   pTierName = "High";   break;
	case cstCustom: pTierName = "Custom"; break;
	}

	Console::WriteLine(ConsoleColor::White, "  [CoStack]");
	Console::WriteLine(ConsoleColor::White, "    size_        : %u KB  (%u pages)",
		_pStack->size_ / 1024, _pStack->size_ / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::White, "    stackTier_   : %d (%s)",
		(_s32)_pStack->stackTier_, pTierName);
	Console::WriteLine(ConsoleColor::White, "    pStackBase_  : 0x%016llX",
		(uintptr_t)_pStack->pStackBase_);
	Console::WriteLine(ConsoleColor::White, "    pStackEnd_   : 0x%016llX",
		(uintptr_t)_pStack->pStackEnd_);
	Console::WriteLine(ConsoleColor::White, "    pStackLimit_ : 0x%016llX",
		(uintptr_t)_pStack->pStackLimit_);
	Console::WriteLine(ConsoleColor::White, "    pGuardLimit_ : 0x%016llX",
		(uintptr_t)_pStack->pGuardLimit_);

	// ── Ranges ────────────────────────────────────────────────────────────────
	long long commitBytes = (long long)(_pStack->pStackBase_ - _pStack->pStackLimit_);
	long long guardBytes  = (long long)(_pStack->pStackLimit_ - _pStack->pGuardLimit_);

	Console::WriteLine(ConsoleColor::White, "  [Ranges]");
	Console::WriteLine(ConsoleColor::White, "    StackRange   : [0x%016llX, 0x%016llX)  (%u KB, %u pages)",
		(uintptr_t)_pStack->pStackEnd_,
		(uintptr_t)_pStack->pStackBase_,
		_pStack->size_ / 1024, _pStack->size_ / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Cyan,  "    CommitRange  : [0x%016llX, 0x%016llX)  (%lld KB, %lld pages)",
		(uintptr_t)_pStack->pStackLimit_,
		(uintptr_t)_pStack->pStackBase_,
		commitBytes / 1024, commitBytes / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Yellow,"    GuardRange   : [0x%016llX, 0x%016llX)  (%lld KB, %lld pages)",
		(uintptr_t)_pStack->pGuardLimit_,
		(uintptr_t)_pStack->pStackLimit_,
		guardBytes / 1024, guardBytes / CO_PAGE_SIZE);

	// ── 페이지 덤프 (높은 주소 → 낮은 주소) ──────────────────────────────────
	size_t pageCount = _pStack->size_ / CO_PAGE_SIZE;
	for (size_t i = 0; i < pageCount; ++i)
	{
		char* pPageBegin = _pStack->pStackBase_ - (i + 1) * CO_PAGE_SIZE;
		char* pPageEnd   = pPageBegin + CO_PAGE_SIZE;

		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery(pPageBegin, &mbi, sizeof(mbi));

		bool isGuard = (mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) != 0);
		if (mbi.State == MEM_FREE)
		{
			Console::WriteLine(ConsoleColor::Red,
				"  [%02llu] 0x%016llX ~ 0x%016llX : FREE",
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else if (isGuard)
		{
			Console::WriteLine(ConsoleColor::Yellow,
				"  [%02llu] 0x%016llX ~ 0x%016llX : COMMIT + GUARD",
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else if (mbi.State == MEM_COMMIT)
		{
			Console::WriteLine(ConsoleColor::Cyan,
				"  [%02llu] 0x%016llX ~ 0x%016llX : COMMIT",
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else
		{
			Console::WriteLine(ConsoleColor::White,
				"  [%02llu] 0x%016llX ~ 0x%016llX : RESERVE",
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Vectored Exception Handler
//   1. 코루틴 스택 가드 페이지 터치 예외처리
//      - STATUS_GUARD_PAGE_VIOLATION 을 가로채서 ExpandStack 호출
//////////////////////////////////////////////////////////////////////////////////////////
LONG CALLBACK CoVEH(EXCEPTION_POINTERS* _pEp)
{
	if (_pEp->ExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;

	char*    pFaultAddr = (char*)_pEp->ExceptionRecord->ExceptionInformation[1];
	CoStack* pStack     = g_cCoMgr.FindStackByAddr(pFaultAddr);
	if (pStack == nullptr)
	{
		jc_assert_msg(false, "가드 페이지 위반이 발생했지만 해당 주소가 관리하는 스택 범위에 없습니다. FaultAddr: 0x%p", pFaultAddr);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	if (!g_cCoMgr.ExpandStack(pStack, pFaultAddr))
		return EXCEPTION_CONTINUE_SEARCH;

	return EXCEPTION_CONTINUE_EXECUTION;
}

//////////////////////////////////////////////////////////////////////////////////////////
// CoAllocCtx / CoFreeCtx  (extern "C" 래퍼)
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoAllocCtx(FnCoroutine _fn, CoStackTier _stackTier, _u32 _stackSize)
{
	if (_fn == nullptr)
	{
		g_cCoLastError = CO_ERROR_NULL_FUNCTION;
		return nullptr;
	}

	CoContext* pCtx = g_cCoMgr.AllocCtx(_stackTier, _stackSize);
	if (pCtx == nullptr)
		return nullptr;

	pCtx->threadId_ = GetCurrentThreadId();
	pCtx->state_    = csInit;
	pCtx->fn_       = _fn;
	return pCtx;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CoFreeCtx(CoContext* _ctx)
{
	if (_ctx == nullptr)
		return;
	g_cCoMgr.FreeCtx(_ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoFindCtxByAddr(char* _pAddr)
{
	return g_cCoMgr.FindContextByAddr(_pAddr);
}
