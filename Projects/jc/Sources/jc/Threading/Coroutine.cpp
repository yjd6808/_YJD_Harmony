/*
 * 작성자 : 윤정도
 * 코루틴 C++ 구현
 * - CoMgr 메서드 전체 (InitStack, AllocStack, FreeStack, InitCtx, AllocCtx, FreeCtx, Clear, FindContextByBase/Addr, FindStackByBase/Addr, ExpandStack, DumpStack)
 * - CoVEH 벡터드 예외 핸들러
 * - CoOnBeforeLaunch/CoOnAfterLaunch
 * - CoCurrentCtx, CoValidateAddr, CoAllocCtx, CoFreeCtx 래퍼
 */

#include "Core.h"
#include "Coroutine.h"

#include <mutex>
#include <intrin.h>
#include <utility>

USING_NS_JC;

// ── 스레드 지역 상태 ──────────────────────────────────────────
// 코루틴의 스레드별 값들을 한곳에 둔다. (CoMgr::tls_alive_ 정의 포함)
// - g_cCoMgr: 스레드별 매니저. live 등록부 + 현재 컨텍스트 + id 발급.
// - tls_coLastError: 마지막 실패 원인. 읽으면 지운다.
// - tls_coGlobalExceptionHandler: 스레드별 전역 예외 핸들러. (개별 onException 다음, 보관 전)
// - tls_coPendingException: CoEntry가 잡아 둔 예외. 스케줄러 스택에서 처리한다.
// - tls_coVehDepth: VEH 재진입 깊이. RAII 스코프로 증감한다.
thread_local CoMgr g_cCoMgr;
thread_local bool CoMgr::tls_alive_ = false;
static thread_local CoError tls_coLastError = coeNone;
static thread_local FnCoException tls_coGlobalExceptionHandler = nullptr;
static thread_local std::exception_ptr tls_coPendingException;
static thread_local _u32 tls_coVehDepth = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// 마지막 실패 원인 보고 (읽으면 지운다)
//////////////////////////////////////////////////////////////////////////////////////////
CoError CoGetLastError()
{
	// 읽으면 지운다. 같은 실패를 두 번 보고하지 않기 위함이다.
	CoError err = tls_coLastError;
	tls_coLastError = coeNone;
	return err;
}

String CoErrorString(CoError _err)
{
	switch (_err)
	{
	case coeNone:				return _T("성공");
	case coeNullFunction:		return _T("코루틴 함수가 null");
	case coeVirtualAlloc:		return _T("스택 예약 실패");
	case coeCommitFailed:		return _T("스택 커밋 실패");
	case coeInvalidStackSize:	return _T("잘못된 스택 크기");
	case coeInvalidCtx:			return _T("유효하지 않은 컨텍스트");
	case coeInvalidState:		return _T("호출 불가능한 상태");
	case coeStaleHandle:		return _T("이미 종료된 핸들");
	case coeException:			return _T("코루틴 함수에서 예외 발생");
	default:					return _T("알려지지 않은 오류");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// CoVEH 자동 등록 + 매니저 수명
//////////////////////////////////////////////////////////////////////////////////////////
static void* s_pCoVeh = nullptr;
static std::once_flag s_coVehOnce;

static void EnsureCoVehRegistered()
{
	std::call_once(s_coVehOnce, []
	{
		s_pCoVeh = ::AddVectoredExceptionHandler(1, CoVEH);
		jc_assert_msg(s_pCoVeh != nullptr, _T("CoVEH 등록 실패"));
	});
}

CoMgr::CoMgr()
{
	tls_alive_ = true;
	// 스레드별 매니저가 처음 만들어지는 시점에 VEH를 보장한다.
	EnsureCoVehRegistered();

	// CET(User Shadow Stack)가 켜진 프로세스에서는 코루틴을 쓸 수 없다.
	// - rsp를 임의로 바꾸고 ret하는 방식이라 섀도우 스택 검사에 걸려 즉사하므로
	//   링커 /CETCOMPAT:NO로 빌드해야 한다. 여기서 미리 막는다.
	jc_assert_msg(!IsShadowStackEnabled(),
		_T("jc 코루틴은 CET(User Shadow Stack)와 호환되지 않는다. /CETCOMPAT:NO로 링크할 것"));
}

CoMgr::~CoMgr()
{
	// 이후 가드 폴트는 모두 스레드 스택의 것으로 보고 OS에 맡긴다.
	// - Clear()에서는 내리지 않는다. Clear는 풀 비우기라 뒤에도 재사용되기 때문이다.
	tls_alive_ = false;
}

bool CoMgr::IsShadowStackEnabled()
{
	PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY policy{};
	if (::GetProcessMitigationPolicy(::GetCurrentProcess(),
		ProcessUserShadowStackPolicy, &policy, sizeof(policy)))
		return policy.EnableUserShadowStack != 0;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 비상 패드 수 계산. 전체의 1/4을 넘지 않게 한다.
//////////////////////////////////////////////////////////////////////////////////////////
// - Low(4p)는 1장, Mid(16p)/High(64p)은 4장(기본값 상한)이 된다.
_u32 CoMgr::EmergencyPadPages(_u32 _totalPages) const
{
	_u32 cap = _totalPages / 4;
	return pageEmergencyCount_ < cap ? pageEmergencyCount_ : cap;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스레드별 전역 예외 핸들러 등록
//////////////////////////////////////////////////////////////////////////////////////////
void CoSetExceptionHandler(FnCoException _fn)
{
	tls_coGlobalExceptionHandler = _fn;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] InitStack
//   CoStack의 pStackEnd_, size_, stackTier_ 가 설정된 상태에서 호출.
//   스택 상단 pageInitCount_ 페이지 commit + pageGuardCount_ 페이지 PAGE_GUARD commit.
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::InitStack(CoStack* _pStack)
{
	_pStack->pStackBase_ = _pStack->pStackEnd_ + _pStack->size_;

	// 경계 확정.
	// - pEmergencyTop_ = pStackEnd_ + PAGE. 이 아래는 일반 확장이 내려가지 못하고,
	//   TEB DeallocationStack에도 이 값을 설치해 커널이 오버플로우로 확정하게 한다.
	// - pReserveBase_ ~ pStackEnd_ = 비상 패드. 미리 RW 커밋해 둔다.
	//   오버플로우 폴트 때 RSP는 예약 하단 근처라 그 아래가 비어 있으면 배달 자체가
	//   죽으므로, 패드가 SEH 디스패치 공간을 보장한다.
	_pStack->pEmergencyTop_ = _pStack->pStackEnd_ + CO_PAGE_SIZE;
	_pStack->overflowed_    = false;

	//  pStackBase_       ──────────────── (높은 주소, 초기 RSP)
	//                    | init pages  |  ← pageInitCount_ 개 commit
	//  pStackLimit_      ────────────────
	//                    | guard pages |  ← pageGuardCount_ 개 commit + PAGE_GUARD
	//  pGuardLimit_      ────────────────
	//                    |  (reserve)  |
	//  pEmergencyTop_    ──────────────── ← 확장 하한 + 커널 오버플로우 경계
	//                    | 오버플로우 가드 1페이지 (GUARD, 절대 해제 금지)
	//  pStackEnd_        ────────────────
	//                    | 비상 패드 N페이지 (RW, 디스패치용. 절대 건드리지 않음)
	//  pReserveBase_     ──────────────── (낮은 주소, 예약 시작. 해제 기준점)

	// init commit 영역: 비상 밴드 상단 미만으로 내려가지 않도록 클램프
	char* pCommitAddr = _pStack->pStackBase_ - (pageInitCount_ * CO_PAGE_SIZE);
	if (pCommitAddr < _pStack->pEmergencyTop_)
		pCommitAddr = _pStack->pEmergencyTop_;

	_u32 actualInitCount = (_u32)((_pStack->pStackBase_ - pCommitAddr) / CO_PAGE_SIZE);
	if (actualInitCount > 0)
	{
		if (VirtualAlloc(pCommitAddr, actualInitCount * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE) == nullptr)
		{
			_LogError_(_T("VirtualAlloc (init commit) failed. Error: %lu"), GetLastError());
			tls_coLastError = coeCommitFailed;
			return false;
		}
	}

	// guard 영역: 비상 밴드 상단 미만으로 내려가지 않도록 클램프
	char* pGuardAddr = pCommitAddr - (pageGuardCount_ * CO_PAGE_SIZE);
	if (pGuardAddr < _pStack->pEmergencyTop_)
		pGuardAddr = _pStack->pEmergencyTop_;

	// 성장 가드존은 PAGE_GUARD로 둔다.
	// - NOACCESS로 두면 커널이 예외 배달용 CONTEXT를 유저 스택에 밀어넣지 못해
	//   VEH가 호출되기도 전에 프로세스가 죽으므로 GUARD여야 한다.
	_u32 actualGuardCount = (_u32)((pCommitAddr - pGuardAddr) / CO_PAGE_SIZE);
	if (actualGuardCount > 0)
	{
		if (VirtualAlloc(pGuardAddr, actualGuardCount * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
		{
			_LogError_(_T("VirtualAlloc (guard commit) failed. Error: %lu"), GetLastError());
			tls_coLastError = coeCommitFailed;
			return false;
		}
	}

	_pStack->pStackLimit_ = pCommitAddr;
	_pStack->pGuardLimit_ = pGuardAddr;
	// 커밋 하한 기록. 풀 반납 때 이 위로만 유지한다.
	_pStack->pCommitLow_ = pGuardAddr;

	// 오버플로우 가드 1페이지를 GUARD로 둔다. (절대 해제 금지)
	// - NOACCESS로 두면 RSP가 바로 위에 있을 때 배달 push가 실패해 죽으므로
	//   GUARD로 둔다. (배달 push가 가드를 자동 해제한다)
	if (VirtualAlloc(_pStack->pStackEnd_, CO_PAGE_SIZE,
		MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
	{
		_LogError_(_T("VirtualAlloc (overflow guard commit) failed. Error: %lu"), GetLastError());
		tls_coLastError = coeCommitFailed;
		return false;
	}

	// 비상 패드(예약 아래 N페이지)를 RW 커밋한다.
	// - 패드 크기는 예약 때 정해진다. (pStackEnd_ - pReserveBase_)
	if (_pStack->pReserveBase_ < _pStack->pStackEnd_)
	{
		if (VirtualAlloc(_pStack->pReserveBase_,
			(SIZE_T)(_pStack->pStackEnd_ - _pStack->pReserveBase_),
			MEM_COMMIT, PAGE_READWRITE) == nullptr)
		{
			_LogError_(_T("VirtualAlloc (emergency pad commit) failed. Error: %lu"), GetLastError());
			tls_coLastError = coeCommitFailed;
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] AllocStack
//   확정된 tier/size로 메모리를 예약(RESERVE)하고 InitStack을 호출.
//   티어 판별은 하지 않는다. (호출 전 ResolveTier로 확정할 것)
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::AllocStack(OUT CoStack* _pStack, CoStackTier _stackTier, _u32 _stackSize)
{
	if (_pStack == nullptr)
	{
		jc_assert_msg(false, _T("스택 구조체 포인터가 nullptr입니다."));
		return false;
	}

	if (_stackTier <= 0 || _stackTier > cstValidTierEnd)
	{
		jc_assert_msg(false, _T("잘못된 스택 티어입니다. tier: %d"), _stackTier);
		return false;
	}

	// ResolveTier를 거친 크기이므로 페이지 정렬이어야 한다.
	jc_assert_msg((_stackSize & (CO_PAGE_SIZE - 1)) == 0,
		_T("스택 크기가 페이지 정렬이 아닙니다. size: %u"), _stackSize);
	if ((_stackSize & (CO_PAGE_SIZE - 1)) != 0)
	{
		tls_coLastError = coeInvalidStackSize;
		return false;
	}

	// 예약 아래에 비상 패드를 덧붙인다. (오버플로우 배달 공간)
	// - 패드는 스택 크기의 1/4을 넘지 않게 한다. (Low는 1장, Mid/High은 4장)
	_u32 padPages = EmergencyPadPages(_stackSize / CO_PAGE_SIZE);
	_u32 slotBytes = _stackSize + padPages * CO_PAGE_SIZE;

	char* pSlot = nullptr;
	if (_stackTier == cstCustom)
	{
		// custom은 기존대로 직접 예약한다. (슬랩에 안 넣음)
		pSlot = (char*)VirtualAlloc(nullptr, slotBytes, MEM_RESERVE, PAGE_NOACCESS);
		if (pSlot == nullptr)
		{
			tls_coLastError = coeVirtualAlloc;
			return false;
		}
	}
	else
	{
		// 슬랩에서 슬롯을 잘라 쓴다. (64KB 그래뉴러리티 낭비 제거)
		// - 1MB 슬랩을 티어 크기로 분할. 빈 슬랩이 없으면 새로 예약한다.
		// - 반납된 슬롯 재사용은 free_ 풀 몫이라 used_는 앞으로만 간다.
		StackSlab* pSlab = nullptr;
		if (!slabs_[_stackTier].IsEmpty())
		{
			StackSlab& back = slabs_[_stackTier].Back();
			if (back.used_ < back.slotCount_)
				pSlab = &back;
		}
		if (pSlab == nullptr)
		{
			char* pSlabBase = (char*)VirtualAlloc(nullptr, SLAB_BYTES, MEM_RESERVE, PAGE_NOACCESS);
			if (pSlabBase == nullptr)
			{
				tls_coLastError = coeVirtualAlloc;
				return false;
			}
			StackSlab slab;
			slab.pBase_ = pSlabBase;
			slab.slotBytes_ = slotBytes;
			slab.slotCount_ = SLAB_BYTES / slotBytes;
			slab.used_ = 0;
			slabs_[_stackTier].PushBack(slab);
			pSlab = &slabs_[_stackTier].Back();
		}
		pSlot = pSlab->pBase_ + (size_t)pSlab->slotBytes_ * pSlab->used_;
		pSlab->used_++;
	}

	_pStack->pReserveBase_ = pSlot;
	_pStack->pStackEnd_  = pSlot + padPages * CO_PAGE_SIZE;
	_pStack->size_       = _stackSize;
	_pStack->stackTier_  = _stackTier;
	_pStack->magic_      = CO_STACK_MAGIC;
	return InitStack(_pStack);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 티어/크기 결정을 한 곳으로 모은다.
//////////////////////////////////////////////////////////////////////////////////////////
static _u32 RoundUpPage(_u32 _value)
{
	return (_value + CO_PAGE_SIZE - 1) & ~(_u32)(CO_PAGE_SIZE - 1);
}

bool CoMgr::ResolveTier(CoStackTier _tier, _u32 _size, OUT CoStackTier* _pTier, OUT _u32* _pSize)
{
	switch (_tier)
	{
	case cstLow:  *_pTier = cstLow;  *_pSize = CO_STACK_SIZE_LOW;  return true;
	case cstMid:  *_pTier = cstMid;  *_pSize = CO_STACK_SIZE_MID;  return true;
	case cstHigh: *_pTier = cstHigh; *_pSize = CO_STACK_SIZE_HIGH; return true;
	case cstCustom:
		if (_size == 0)
		{
			tls_coLastError = coeInvalidStackSize;
			return false;
		}
		// 작은 요청은 티어로 올림. 크기도 티어 크기로 맞춰 풀에 섞이지 않게 한다.
		if (_size <= CO_STACK_SIZE_LOW)  { *_pTier = cstLow;  *_pSize = CO_STACK_SIZE_LOW;  return true; }
		if (_size <= CO_STACK_SIZE_MID)  { *_pTier = cstMid;  *_pSize = CO_STACK_SIZE_MID;  return true; }
		if (_size <= CO_STACK_SIZE_HIGH) { *_pTier = cstHigh; *_pSize = CO_STACK_SIZE_HIGH; return true; }
		// 진짜 custom: 페이지 단위로 올림, 풀링 없음.
		*_pTier = cstCustom;
		*_pSize = RoundUpPage(_size);
		return true;
	default:
		jc_assert_msg(false, _T("잘못된 스택 티어입니다. tier: %d"), _tier);
		return false;
	}
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
		// 패드까지 포함해 예약 전체를 해제한다.
		VirtualFree(_pStack->pReserveBase_, 0, MEM_RELEASE);
	}
	else
	{
		VirtualFree(_pStack->pBaseAddr_, _pStack->size_, MEM_DECOMMIT);
	}
	_pStack->pStackLimit_ = nullptr;
	_pStack->pGuardLimit_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 풀 반납 시 커밋 유지 + 가드존 재배치.
// - 이전에는 매번 전체 decommit + 재commit(커널 3회)이라 생성 1회에 수 µs가 들었다.
// - 유지 상한(poolKeepBytes_)을 넘는 커밋만 디커밋하고, 가드존은 Protect로만
//   재배치한다. 확장 없이 끝난 경우(가장 흔함)는 Protect 1회로 끝난다.
// - 커밋된 페이지 내용은 지우지 않는다. 다음 사용자가 바로 쓴다.
//   (스택 잔류 데이터가 문제면 SecureZeroMemory 정책을 추가할 것)
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::RecycleStack(CoStack* _pStack)
{
	const _u32 guardBytes = CO_PAGE_SIZE * pageGuardCount_;
	const _u32 initBytes  = CO_PAGE_SIZE * pageInitCount_;

	// 유지 상한을 넘는 커밋만 디커밋한다. (보통은 0바이트)
	// - 먼저 잘라야 아래 pCommitLow_가 실제 커밋 하한을 가리킨다.
	char* pKeepLow = _pStack->pStackBase_ - poolKeepBytes_;
	if (_pStack->pCommitLow_ != nullptr && _pStack->pCommitLow_ < pKeepLow)
	{
		VirtualFree(_pStack->pCommitLow_, (SIZE_T)(pKeepLow - _pStack->pCommitLow_), MEM_DECOMMIT);
		_pStack->pCommitLow_ = pKeepLow;
	}

	// 위치 재계산. 실제 커밋 하한 아래로 내려가지 않는다.
	// - 잘라낸 뒤에도 예전 위치를 가리키면 다음 사용자가 디커밋된 곳을 밟아 죽는다.
	//   (Eager로 전체 커밋 후 잘리면 limit이 허공을 가리키던 버그)
	char* pFloor = _pStack->pEmergencyTop_;
	if (_pStack->pCommitLow_ != nullptr && pFloor < _pStack->pCommitLow_)
		pFloor = _pStack->pCommitLow_;
	char* pInitLow = _pStack->pStackBase_ - initBytes;
	if (pInitLow < pFloor)
		pInitLow = pFloor;
	char* pGuardLow = pInitLow - guardBytes;
	if (pGuardLow < pFloor)
		pGuardLow = pFloor;

	// 실제 가드존 크기. 클램프되면 guardBytes보다 작다.
	// - 클램프 무시하고 guardBytes로 Protect하면 init 영역까지 가드가 번진다.
	SIZE_T guardZoneBytes = (SIZE_T)(pInitLow - pGuardLow);

	// 가드존을 초기 위치로. 이미 커밋된 영역이라 Protect만으로 된다.
	// - 확장 없이 끝났으면 가드가 그대로 살아 있어서 이것도 생략한다. (커널 0회)
	if (_pStack->pStackLimit_ == pInitLow && _pStack->pGuardLimit_ == pGuardLow)
		return;
	if (guardZoneBytes == 0)
	{
		_pStack->pStackLimit_ = pInitLow;
		_pStack->pGuardLimit_ = pGuardLow;
		return;
	}

	DWORD old = 0;
	if (::VirtualProtect(pGuardLow, guardZoneBytes, PAGE_READWRITE | PAGE_GUARD, &old) != FALSE)
	{
		_pStack->pStackLimit_ = pInitLow;
		_pStack->pGuardLimit_ = pGuardLow;
		return;
	}

	// Protect 실패 = 가드존이 디커밋된 상태. 커밋부터 다시 한다.
	if (VirtualAlloc(pGuardLow, guardZoneBytes, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) != nullptr)
	{
		_pStack->pStackLimit_ = pInitLow;
		_pStack->pGuardLimit_ = pGuardLow;
		if (_pStack->pCommitLow_ == nullptr || pGuardLow < _pStack->pCommitLow_)
			_pStack->pCommitLow_ = pGuardLow;
	}
	// 여기까지 실패하면 다음 AllocCtx가 InitStack... (풀 경로는 InitStack을 안 타므로
	//  가드 없이 돌아간다. 다음 확장이 커널 몫이 되는 기존 한계와 동일.assert로 알림)
	jc_assert_msg(_pStack->pGuardLimit_ == pGuardLow, _T("가드존 재설치 실패"));
}

//////////////////////////////////////////////////////////////////////////////////////////
// InitCtx
//   풀에서 꺼낸 CoContext를 가볍게 리셋. 스택은 반납 때 이미 다음 사용 준비가
//   끝나서 커밋을 다시 안 한다.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::InitCtx(CoContext* _pCtx)
{
	_pCtx->id_         = 0;
	_pCtx->regs_       = {};
	_pCtx->callerRegs_ = {};
	_pCtx->state_      = csInit;
	_pCtx->fn_         = nullptr;
	_pCtx->pCallerCtx_ = nullptr;
	// 사용자 채널은 재사용 때마다 비운다. (id는 AllocCtx에서 재부여)
	_pCtx->userData_        = nullptr;
	_pCtx->switchData_      = 0;
	_pCtx->isCancelRequested_ = false;
	_pCtx->onException_     = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// AllocCtx
//   풀에 재사용 가능한 CoContext가 있으면 꺼내서 InitCtx 후 반환.
//   없으면 새로 dbg_new 하고 AllocStack → usingByBase_/usingById_ 등록 후 반환.
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::AllocCtx(CoStackTier _stackTier, _u32 _stackSize)
{
	// 티어/크기 판별을 한 곳에서 확정한다.
	CoStackTier tier = cstNone;
	_u32 size = 0;
	if (!ResolveTier(_stackTier, _stackSize, &tier, &size))
		return nullptr;

	CoContext* pCtx = nullptr;

	if (tier != cstCustom && free_[tier].PopFront(&pCtx))
	{
		// 풀 재사용: 커밋 유지됨. 레지스터/상태만 리셋하고 바로 쓴다.
		// 풀 무결성 확인. 크기가 다르면 오염된 것이므로 쓰지 않는다.
		jc_assert_msg(pCtx->stack_.size_ == size,
			_T("풀 오염: 티어 크기와 다릅니다. tier: %d, size: %u"), tier, pCtx->stack_.size_);
		InitCtx(pCtx);
	}
	else
	{
		pCtx = dbg_new CoContext();
		if (!AllocStack(&pCtx->stack_, tier, size))
		{
			delete pCtx;
			return nullptr;
		}
		pCtx->state_ = csInit;
	}

	// movaps 전제. CoContext가 16 정렬이어야 regs_ 안 XMM도 정렬된다.
	jc_assert_msg((((uintptr_t)pCtx & 15) == 0),
		_T("CoContext가 16 정렬이 아닙니다. pCtx: 0x%p"), pCtx);

	pCtx->id_ = ++nextId_;
	usingByBase_.Insert(pCtx->stack_.pStackBase_, pCtx);
	usingById_.Insert(pCtx->id_, pCtx);
	return pCtx;
}

//////////////////////////////////////////////////////////////////////////////////////////
// FreeCtx
//   usingByBase_/usingById_ 에서 제거 후 커스텀이면 메모리 해제 + delete,
//   풀 티어면 decommit 후 free_ 풀로 반환.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::FreeCtx(CoContext* _pCtx)
{
	jc_assert(_pCtx != nullptr);

	// 실행 중인 컨텍스트를 풀로 되돌리는 건 버그다.
	jc_assert(currentCtx_ != _pCtx);

	CoStackTier stackTier = _pCtx->stack_.stackTier_;
	jc_assert_msg(stackTier >= cstValidTierBegin && stackTier <= cstValidTierEnd,
		_T("잘못된 스택 티어입니다. tier: %d"), stackTier);

	CoContext* pPopped = nullptr;
	if (usingByBase_.TryPop(_pCtx->stack_.pStackBase_, &pPopped) == false)
	{
		jc_assert_msg(false, _T("해당 컨텍스트는 관리 중인 컨텍스트가 아닙니다. pStackBase_: 0x%p"),
			_pCtx->stack_.pStackBase_);
		return;
	}
	CoContext* pPoppedById = nullptr;
	if (usingById_.TryPop(pPopped->id_, &pPoppedById) == false || pPoppedById != pPopped)
	{
		jc_assert_msg(false, _T("id 맵과 base 맵이 어긋났습니다. id: %llu"), pPopped->id_);
		return;
	}

	pPopped->state_ = csEnd;
	pPopped->fn_    = nullptr;

	if (stackTier == cstCustom)
	{
		FreeStack(&pPopped->stack_);
		delete pPopped;
	}
	else if (free_[stackTier].Size() >= (int)poolMax_[stackTier])
	{
		// 풀이 가득 찼다. 슬롯은 슬랩 조각이라 release하면 안 되고
		// decommit 후 버린다. (슬랩 자체는 Clear 때 해제)
		FreeStack(&pPopped->stack_);
		delete pPopped;
	}
	else
	{
		// 커밋을 유지한 채 풀로 되돌린다. 다음 AllocCtx는 커널을 안 탄다.
		RecycleStack(&pPopped->stack_);
		free_[stackTier].PushBack(pPopped);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Clear
//   usingByBase_ 에 남은 항목 경고 후 free_ 풀 전체 해제.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::Clear()
{
	currentCtx_ = nullptr;

	if (usingByBase_.Size() > 0)
	{
		jc_assert_msg(false, _T("Clear 호출 시점에 아직 할당된 컨텍스트가 존재합니다. Count: %zu"), usingByBase_.Size());
	}

	for (_s32 tier = cstReservedTierBegin; tier <= cstReservedTierEnd; ++tier)
	{
		CoContext* pCtx = nullptr;
		while (free_[tier].PopFront(&pCtx))
		{
			// 풀 ctx는 슬랩 슬롯이라 개별 release 금지. delete만 한다.
			// (custom은 풀에 안 들어가서 여기 올 일이 없음)
			delete pCtx;
		}
	}

	// 슬랩 예약 전체를 해제한다.
	for (_s32 tier = cstReservedTierBegin; tier <= cstReservedTierEnd; ++tier)
	{
		StackSlab slab;
		while (slabs_[tier].PopFront(&slab))
			VirtualFree(slab.pBase_, 0, MEM_RELEASE);
	}

	// id 맵은 비어 있어야 한다. (살아있는 ctx는 위 assert에서 걸림)
	// nextId_는 리셋하지 않는다. 리셋하면 id가 재사용되어 ABA가 부활한다.
	usingById_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// FindContextByBase / FindContextByAddr / FindContextById
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::FindContextByBase(char* _pBase)
{
	CoContext** pFound = usingByBase_.Find(_pBase);
	if (pFound == nullptr)
		return nullptr;
	return *pFound;
}

CoContext* CoMgr::FindContextByAddr(char* _pAddr)
{
	CoContext** pFound = usingByBase_.LowerBoundValue(_pAddr);
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
	CoContext** pFound = usingByBase_.Find(_pBase);
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

CoContext* CoMgr::FindContextById(CoId _id)
{
	CoContext** pFound = usingById_.Find(_id);
	if (pFound == nullptr)
		return nullptr;
	return *pFound;
}

bool CoMgr::IsUsing(CoContext* _pCtx)
{
	if (_pCtx == nullptr)
		return false;
	CoContext** pFound = usingByBase_.Find(_pCtx->stack_.pStackBase_);
	return pFound != nullptr && *pFound == _pCtx;
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
// ※ 주의: 여기서는 스택확장이 발생한다.
// 확장 전에 Console::WriteLine 등, chkstk를 다시 호출하여 VEH가 재귀적으로 호출되는 케이스가 발생할 수 있으므로.
// 로그를 남기고 싶다면 확장이 다 된 이후에 하도록 할 것.
// VEH에서 호출: 가드 페이지 터치 시 스택 확장 처리
//
// 처리 흐름:
//   [Before]                              [After]
//   pStackLimit_                           pStackLimit_ (old)
//    GUARD page 0  ← _pFaultAddr            RW page 0       ← fault 페이지 커밋
//    GUARD page 1                           RW grow 0       ← pageGrowCount_ 개 함께 커밋
//    GUARD page 2                           RW grow 1
//   pGuardLimit_                         pStackLimit_ (new) = 성장 시작 주소
//    (reserved)                             NEW GUARD 0      ← pageGuardCount_ 페이지 새로
//    (reserved)                             NEW GUARD 1         commit + PAGE_GUARD 설정
//    (reserved)                             NEW GUARD 2
//   pStackEnd_                           pGuardLimit_ (new)
//                                          (reserved)
//                                         pStackEnd_
// 확장이 끝나면 TEB StackLimit도 함께 내린다.
// - 이전에는 pStackLimit_만 내려가고 gs:[16]이 그대로라 확장된 영역에서
//   예외를 던지면 SEH가 스택 범위를 벗어났다고 보고 프로세스를 죽였다.
//////////////////////////////////////////////////////////////////////////////////////////
bool __declspec(safebuffers) CoMgr::ExpandStack(CoContext* _pCtx, char* _pFaultAddr)
{
	CoStack* _pStack = &_pCtx->stack_;
	if (pageGuardCount_ == 0)
	{
		// 페이지 가드 갯수가 없는 경우는 존재할 수 없다.
		// 말도안되는 경우 이므로 크래쉬.
		return false;
	}

	// 비상 밴드 상단이 일반 확장이 내려갈 수 있는 하한이다.
	// - 밴드 아래 폴트는 오버플로우로 보고 false를 돌려준다.
	if (_pFaultAddr < _pStack->pEmergencyTop_)
		return false;

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

		// 비상 밴드 위로 클램프 (밴드는 확장 대상이 아님)
		if (pGrowthStart < _pStack->pEmergencyTop_)
			pGrowthStart = _pStack->pEmergencyTop_;

		commitPageCount += (_u32)((pGrowthEnd - pGrowthStart) / CO_PAGE_SIZE);
	}

	if (VirtualAlloc(pGrowthStart, CO_PAGE_SIZE * commitPageCount, MEM_COMMIT, PAGE_READWRITE) == nullptr)
		return false;

	_pStack->pStackLimit_  = pGrowthStart;
	char* pNewGuardEnd     = pGrowthStart;
	char* pNewGuardStart   = pGrowthStart - pageGuardCount_ * CO_PAGE_SIZE;

	// 새 가드존도 비상 밴드 위까지만 둔다.
	if (pNewGuardStart < _pStack->pEmergencyTop_)
		pNewGuardStart = _pStack->pEmergencyTop_;

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

	// 커밋 하한 갱신. (새 가드존까지 커밋됨)
	if (_pStack->pCommitLow_ == nullptr || pNewGuardStart < _pStack->pCommitLow_)
		_pStack->pCommitLow_ = pNewGuardStart;

	// 이 코루틴이 지금 실행 중(폴트를 낸 스택)이면 TEB도 함께 내린다.
	// - VEH는 폴트를 낸 스레드에서 돌므로 이 TEB 쓰기가 정확하다.
	// - 예외 디스패처가 보는 유효 스택 = [pStackLimit_, pStackBase_) 이다.
	if (_pCtx == currentCtx_)
		__writegsqword(0x10, (_u64)pGrowthStart);

	//_LogInfo_("Stack expanded: newLimit=0x%p  newGuardLimit=0x%p  growthSize=%u KB (%u pages)",
	//	_pStack->pStackLimit_, _pStack->pGuardLimit_,
	//	(uint32_t)(pFaultEnd - pFaultStart) / 1024, (uint32_t)(pFaultEnd - pFaultStart) / CO_PAGE_SIZE);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// DumpStack
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::DumpStack(CoStack* _pStack, const _char* _pTitle /*= nullptr*/)
{
	if (_pTitle)
		Console::WriteLine(ConsoleColor::Green, _T("\n==== %s ===="), _pTitle);

	// ── CoStack 멤버 ──────────────────────────────────────────────────────────
	const _char* pTierName = _T("Unknown");
	switch (_pStack->stackTier_)
	{
	case cstNone:   pTierName = _T("None");   break;
	case cstLow:    pTierName = _T("Low");    break;
	case cstMid:    pTierName = _T("Mid");    break;
	case cstHigh:   pTierName = _T("High");   break;
	case cstCustom: pTierName = _T("Custom"); break;
	}

	Console::WriteLine(ConsoleColor::White, _T("  [CoStack]"));
	Console::WriteLine(ConsoleColor::White, _T("    size_        : %u KB  (%u pages)"),
		_pStack->size_ / 1024, _pStack->size_ / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::White, _T("    stackTier_   : %d (%s)"),
		(_s32)_pStack->stackTier_, pTierName);
	Console::WriteLine(ConsoleColor::White, _T("    pStackBase_  : 0x%016llX"),
		(uintptr_t)_pStack->pStackBase_);
	Console::WriteLine(ConsoleColor::White, _T("    pStackEnd_   : 0x%016llX"),
		(uintptr_t)_pStack->pStackEnd_);
	Console::WriteLine(ConsoleColor::White, _T("    pStackLimit_ : 0x%016llX"),
		(uintptr_t)_pStack->pStackLimit_);
	Console::WriteLine(ConsoleColor::White, _T("    pGuardLimit_ : 0x%016llX"),
		(uintptr_t)_pStack->pGuardLimit_);

	// ── Ranges ────────────────────────────────────────────────────────────────
	_s64 commitBytes = (_s64)(_pStack->pStackBase_ - _pStack->pStackLimit_);
	_s64 guardBytes  = (_s64)(_pStack->pStackLimit_ - _pStack->pGuardLimit_);

	Console::WriteLine(ConsoleColor::White, _T("  [Ranges]"));
	Console::WriteLine(ConsoleColor::White, _T("    StackRange   : [0x%016llX, 0x%016llX)  (%u KB, %u pages)"),
		(uintptr_t)_pStack->pStackEnd_,
		(uintptr_t)_pStack->pStackBase_,
		_pStack->size_ / 1024, _pStack->size_ / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Cyan,  _T("    CommitRange  : [0x%016llX, 0x%016llX)  (%lld KB, %lld pages)"),
		(uintptr_t)_pStack->pStackLimit_,
		(uintptr_t)_pStack->pStackBase_,
		commitBytes / 1024, commitBytes / CO_PAGE_SIZE);
	Console::WriteLine(ConsoleColor::Yellow, _T("    GuardRange   : [0x%016llX, 0x%016llX)  (%lld KB, %lld pages)"),
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
				_T("  [%02llu] 0x%016llX ~ 0x%016llX : FREE"),
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else if (isGuard)
		{
			Console::WriteLine(ConsoleColor::Yellow,
				_T("  [%02llu] 0x%016llX ~ 0x%016llX : COMMIT + GUARD"),
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else if (mbi.State == MEM_COMMIT)
		{
			Console::WriteLine(ConsoleColor::Cyan,
				_T("  [%02llu] 0x%016llX ~ 0x%016llX : COMMIT"),
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
		else
		{
			Console::WriteLine(ConsoleColor::White,
				_T("  [%02llu] 0x%016llX ~ 0x%016llX : RESERVE"),
				i, (uintptr_t)pPageBegin, (uintptr_t)pPageEnd);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Vectored Exception Handler
//   코루틴 스택 가드 페이지 터치 예외처리
//
// [오버플로우 가드 페이지]
//   pStackEnd_ ~ pStackEnd_+PAGE 는 영구 가드 페이지.
//   이 페이지가 터치되면 ExpandStack이 false 반환 → STACK_OVERFLOW 예외로 변환.
//   EXCEPTION_NONCONTINUABLE 플래그를 세워 실행 재개가 불가능함을 명시.
//
// [재귀 진입 처리]
//   재진입 깊이를 카운터+RAII로 센다. (CoVehScope)
//   재귀 진입 시 currentCtx_ 범위 체크만 수행하고 CONTINUE_EXECUTION 반환.
//   가드존 재설치는 최상위 CoVEH 호출의 ExpandStack에서 일괄 처리됨.
//
//   코루틴 실행 중이 아닌 경우(currentCtx_==nullptr)는 즉시 CONTINUE_SEARCH.
//////////////////////////////////////////////////////////////////////////////////////////
struct CoVehScope
{
	_u32& depth_;
	explicit CoVehScope(_u32& _d) noexcept : depth_(_d) { ++depth_; }
	~CoVehScope() noexcept { --depth_; }
};

static inline bool CoIsGrowableGuard(const CoContext* _pCtx, const char* _pAddr) noexcept
{
	return _pAddr >= _pCtx->stack_.pGuardLimit_ && _pAddr < _pCtx->stack_.pStackLimit_;
}

static inline bool CoIsOverflowGuard(const CoContext* _pCtx, const char* _pAddr) noexcept
{
	return _pAddr >= _pCtx->stack_.pStackEnd_ && _pAddr < _pCtx->stack_.pEmergencyTop_;
}

static inline bool CoIsOnStack(const CoContext* _pCtx, const char* _pAddr) noexcept
{
	return _pAddr >= _pCtx->stack_.pStackEnd_ && _pAddr < _pCtx->stack_.pStackBase_;
}

static LONG CoConvertToOverflow(EXCEPTION_POINTERS* _pEp, CoContext* _pCtx) noexcept
{
	if (_pCtx != nullptr)
		_pCtx->stack_.overflowed_ = true;
	_pEp->ExceptionRecord->ExceptionCode   = STATUS_STACK_OVERFLOW;
	_pEp->ExceptionRecord->ExceptionFlags |= EXCEPTION_NONCONTINUABLE;
	return EXCEPTION_CONTINUE_SEARCH;
}

// VEH 경로 함수는 스택을 적게 쓴다. 보안 쿠키 검사가 들어갈
// 로컬 배열·문자열 포맷을 두지 않는다. (프레임 크기는 맵 파일에서 확인)
#define CO_VEH_PATH __declspec(safebuffers)

CO_VEH_PATH __declspec(noinline) LONG CALLBACK CoVEH(EXCEPTION_POINTERS* _pEp) noexcept
{
	// 1. 관심사만 남긴다. (이 핸들러는 모든 예외에 불리므로 싸야 한다)
	const EXCEPTION_RECORD* pRec = _pEp->ExceptionRecord;
	if (pRec->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;
	if (pRec->NumberParameters < 2)
		return EXCEPTION_CONTINUE_SEARCH;
	if (!CoMgr::tls_alive_)
		return EXCEPTION_CONTINUE_SEARCH;

	char* pFaultAddr = (char*)pRec->ExceptionInformation[1];
	char* pFaultRsp  = (char*)_pEp->ContextRecord->Rsp;

#ifdef _DEBUG
	// VEH가 실제로 쓴 깊이를 잰다. (가드 예산 튜닝 근거)
	{
		char* pVehRsp = (char*)_AddressOfReturnAddress();
		char* pFaultRspDbg = (char*)_pEp->ContextRecord->Rsp;
		CoMgr& mgrDbg = g_cCoMgr;
		size_t dispatchUsed = (size_t)(pFaultRspDbg - pVehRsp);
		CoContext* pDbg = mgrDbg.GetCurrentCtx();
		if (pDbg != nullptr)
		{
			size_t remain = (size_t)(pVehRsp - pDbg->stack_.pGuardLimit_);
			if (dispatchUsed > mgrDbg.vehStats_.maxDispatchUsed_)
				mgrDbg.vehStats_.maxDispatchUsed_ = dispatchUsed;
			if (remain < mgrDbg.vehStats_.minRemain_)
				mgrDbg.vehStats_.minRemain_ = remain;
		}
	}
#endif

	// 2. 대상 ctx를 정한다. currentCtx_ 우선, 비어 있으면 주소로 역조회한다.
	// (스위치 직후 CoOnBeforeLaunch 전 몇 명령 사이를 보완한다)
	CoContext* pCtx = g_cCoMgr.GetCurrentCtx();
	if (pCtx == nullptr || !CoIsOnStack(pCtx, pFaultAddr))
	{
		if (!g_cCoMgr.TryFindContextByAddr(pFaultAddr, &pCtx))
			return EXCEPTION_CONTINUE_SEARCH;
	}
	if (pCtx->stack_.magic_ != CO_STACK_MAGIC)
	{
		__debugbreak();
		return EXCEPTION_CONTINUE_SEARCH;
	}
	// 실행 중이 아니면 스택이 자랄 수 없으므로 건드리지 않는다.
	if (pCtx->state_ != csRun && pCtx->state_ != csInit)
		return EXCEPTION_CONTINUE_SEARCH;
	// 스택을 "써서" 난 폴트가 아니면(다른 스레드가 읽은 경우 등) 건드리지 않는다.
	if (!CoIsOnStack(pCtx, pFaultRsp))
		return EXCEPTION_CONTINUE_SEARCH;

	// 3. 오버플로우 가드는 즉시 확정한다.
	if (CoIsOverflowGuard(pCtx, pFaultAddr))
		return CoConvertToOverflow(_pEp, pCtx);
	// 우리가 설치한 성장 가드가 아니면(사용자 PAGE_GUARD 등) 건드리지 않는다.
	if (!CoIsGrowableGuard(pCtx, pFaultAddr))
		return EXCEPTION_CONTINUE_SEARCH;

	// 4. 재진입: 바깥 호출이 가드존을 일괄 재설치하므로 실행만 재개한다.
	// 2중 이상은 공간이 진짜 없다는 뜻이라 오버플로우로 확정한다.
	CoVehScope scope(tls_coVehDepth);
	if (tls_coVehDepth > 1)
	{
		if (tls_coVehDepth > 2)
			return CoConvertToOverflow(_pEp, pCtx);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	// 5. 확장. 실패하면 오버플로우로 확정한다. (조용히 넘기면 다음 페이지에서 AV가 난다)
	// TEB StackLimit 갱신은 ExpandStack 안에서 한다.
	if (g_cCoMgr.ExpandStack(pCtx, pFaultAddr))
		return EXCEPTION_CONTINUE_EXECUTION;
	return CoConvertToOverflow(_pEp, pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// currentCtx_ 체인 관리
//   currentCtx_/pCallerCtx_ 갱신은 CoRun/CoResume/CoYield 래퍼에서 직접 한다.
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
// 스레드 스택으로 돌아온 뒤 가드존이 살아있는지 확인한다.
// - 가드 비트까지는 보지 않는다. 커널이 성장 과정에서 우리 가드를 조용히
//   커밋해 버리기 때문이다. 커밋 자체가 풀렸으면 풀 관리 버그다.
void CoMgr::VerifyGuardZone(const CoStack& _stack) noexcept
{
	if (_stack.pGuardLimit_ >= _stack.pStackLimit_)
		return;
	MEMORY_BASIC_INFORMATION mbi{};
	if (::VirtualQuery(_stack.pGuardLimit_, &mbi, sizeof(mbi)) == sizeof(mbi))
	{
		if (mbi.State != MEM_COMMIT)
			__debugbreak();
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// CoCurrentCtx / CoValidateAddr  (extern "C" 래퍼)
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoCurrentCtx()
{
	return g_cCoMgr.GetCurrentCtx();
}

bool CoValidateAddr(CoContext* _pCtx, char* _pAddr)
{
	if (_pCtx == nullptr)
		return false;
	return _pAddr >= _pCtx->stack_.pStackEnd_
		&& _pAddr <  _pCtx->stack_.pStackBase_;
}

//////////////////////////////////////////////////////////////////////////////////////////
// id 기반 실행 (검증 + 재개 + 시작 + 스코프 가드)
//////////////////////////////////////////////////////////////////////////////////////////
// id로 재개 가능한지 검사한다. 검증 순서: 맵 미조회 → stale, 자기 자신 → invalid, csYield 아님 → invalid.
// - 타 스레드 코루틴은 맵에 안 보이므로 stale로 보고된다. (잘못된 점프 방지는 동일)
static bool CoLookupResumable(CoId _id, OUT CoContext** _pOut)
{
	CoContext* pCtx = g_cCoMgr.FindContextById(_id);
	if (pCtx == nullptr)
	{
		tls_coLastError = coeStaleHandle;
		return false;
	}
	if (pCtx == g_cCoMgr.GetCurrentCtx())
	{
		// 자기 자신을 resume하면 자기 스택으로 재진입해 스택이 오염된다.
		tls_coLastError = coeInvalidState;
		return false;
	}
	if (pCtx->state_ != csYield)
	{
		tls_coLastError = coeInvalidState;
		return false;
	}
	if (_pOut != nullptr)
		*_pOut = pCtx;
	return true;
}

// 스위치 복귀 뒷처리. yield면 true, 종료면 정리 후 false를 돌려준다.
// - 종료 시 보관 예외가 있으면 onException → 전역 → 보관 순으로 처리한다.
//   핸들러에는 소유권을 넘겨 정확히 1회 전달하며, 호출은 스케줄러 스택에서 일어난다.
static bool CoOnSwitchReturn(CoId _id, CoContext* _pCtx)
{
	if (_pCtx->state_ != csEnd)
		return true;

	FnCoException onException = _pCtx->onException_;
	g_cCoMgr.FreeCtx(_pCtx);

	if (tls_coPendingException)
	{
		std::exception_ptr pending = tls_coPendingException;
		if (onException != nullptr || tls_coGlobalExceptionHandler != nullptr)
			tls_coPendingException = nullptr; // 소비형 전달. 핸들러 복귀 후 보관에 남기지 않음
		if (onException != nullptr)
			onException(_id, pending);
		else if (tls_coGlobalExceptionHandler != nullptr)
			tls_coGlobalExceptionHandler(_id, pending);
		// 둘 다 없으면 보관 유지. CoTakePendingException()으로 꺼낸다.
	}
	return false;
}

bool CoResume(CoId _id)
{
	CoContext* pCtx = nullptr;
	if (!CoLookupResumable(_id, &pCtx))
		return false;
	CoMgr& mgr = g_cCoMgr;
	pCtx->pCallerCtx_ = mgr.currentCtx_;
	mgr.currentCtx_ = pCtx;
	pCtx->state_ = csRun;
	CoSwitchImpl(&pCtx->callerRegs_, &pCtx->regs_);
	// 복귀: yield 또는 종료. 스케줄러 쪽 currentCtx_를 되돌린다.
	mgr.currentCtx_ = pCtx->pCallerCtx_;
#ifdef _DEBUG
	CoMgr::VerifyGuardZone(pCtx->stack_);
#endif
	return CoOnSwitchReturn(_id, pCtx);
}

bool CoResume(CoId _id, _u64 _inSwitchData, _u64* _pOut)
{
	CoContext* pCtx = nullptr;
	if (!CoLookupResumable(_id, &pCtx))
	{
		if (_pOut != nullptr)
			*_pOut = 0;
		return false;
	}
	pCtx->switchData_ = _inSwitchData;
	CoMgr& mgr = g_cCoMgr;
	pCtx->pCallerCtx_ = mgr.currentCtx_;
	mgr.currentCtx_ = pCtx;
	pCtx->state_ = csRun;
	CoSwitchImpl(&pCtx->callerRegs_, &pCtx->regs_);
	mgr.currentCtx_ = pCtx->pCallerCtx_;
#ifdef _DEBUG
	CoMgr::VerifyGuardZone(pCtx->stack_);
#endif
	bool alive = CoOnSwitchReturn(_id, pCtx);
	if (_pOut != nullptr)
		*_pOut = alive ? pCtx->switchData_ : 0;
	return alive;
}

CoId CoRun(FnCoroutine _fn, const CoDesc& _desc)
{
	if (_fn == nullptr)
	{
		tls_coLastError = coeNullFunction;
		return CO_INVALID_ID;
	}

	CoContext* pCtx = g_cCoMgr.AllocCtx(_desc.spec_.tier_, _desc.spec_.size_);
	if (pCtx == nullptr)
		return CO_INVALID_ID;

	pCtx->state_    = csInit;
	pCtx->fn_       = _fn;
	pCtx->userData_ = _desc.userData_;
	pCtx->onException_ = _desc.onException_;
	CoId id = pCtx->id_;
	if (_desc.ppOut_ != nullptr)
		*_desc.ppOut_ = pCtx;

	// TEB에 설치할 코루틴 스택 범위. (실제 교체는 CoSwitchImpl이 한다)
	char* pBase = pCtx->stack_.pStackBase_;
	pCtx->regs_.gs8_ = (_u64)pBase;
	pCtx->regs_.gs16_ = (_u64)pCtx->stack_.pStackLimit_;
	pCtx->regs_.gs1478_ = (_u64)pCtx->stack_.pEmergencyTop_;
	// 부동소수점 제어 상태는 기본값으로 시작한다. (스레드값 상속 안 함)
	// regs_는 InitCtx에서 {} 리셋되어 mxcsr_=0x1F80, fpucw_=0x027F임.
	// thunk가 ctx를 찾을 통로. (thunk: mov rcx, rbp)
	pCtx->regs_.rbp_ = (_u64)pCtx;
	// 스택 준비: [base-8] = 0 (가짜 반환주소. 언와인더 종료),
	// [base-16] = thunk 주소, [base-24] = 더미 (CoSwitchImpl의 pop rbx용).
	*(void**)(pBase - 8) = nullptr;
	*(void**)(pBase - 16) = (void*)&CoEntryThunk;
	*(void**)(pBase - 24) = nullptr;
	pCtx->regs_.rsp_ = (_u64)(pBase - 24);

	CoMgr& mgr = g_cCoMgr;
	CoContext* pParent = mgr.currentCtx_;
	pCtx->pCallerCtx_ = pParent;
	mgr.currentCtx_ = pCtx;
	pCtx->state_ = csRun;
	CoSwitchImpl(&pCtx->callerRegs_, &pCtx->regs_);
	mgr.currentCtx_ = pParent;
#ifdef _DEBUG
	CoMgr::VerifyGuardZone(pCtx->stack_);
#endif
	if (pCtx->state_ != csEnd)
		return id;
	return CoOnSwitchReturn(id, pCtx) ? id : CO_INVALID_ID;
}

CoScope::~CoScope()
{
	CoContext* pCtx = g_cCoMgr.FindContextById(id_);
	if (pCtx == nullptr)
	{
		id_ = CO_INVALID_ID;
		return;
	}
#ifdef _DEBUG
	if (rule_ == CoScopeRule::CancelAssert)
		jc_assert_msg(false, _T("CoScope: 명시 종료 없이 소멸합니다. id: %llu"), id_);
#endif
	// 소멸 중 핸들러 재throw는 terminate된다. (noexcept 소멸자)
	Cancel();
}

void CoScope::Cancel()
{
	CoContext* pCtx = g_cCoMgr.FindContextById(id_);
	if (pCtx == nullptr)
	{
		id_ = CO_INVALID_ID;
		return;
	}
	pCtx->isCancelRequested_ = true;
	while (CoResume(id_)) {}
	id_ = CO_INVALID_ID;
}

//////////////////////////////////////////////////////////////////////////////////////////
// CoAllocCtx / CoFreeCtx  (extern "C" 래퍼)
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoAllocCtx(FnCoroutine _fn, CoStackTier _stackTier, _u32 _stackSize)
{
	if (_fn == nullptr)
	{
		tls_coLastError = coeNullFunction;
		return nullptr;
	}

	CoContext* pCtx = g_cCoMgr.AllocCtx(_stackTier, _stackSize);
	if (pCtx == nullptr)
		return nullptr;

	pCtx->state_    = csInit;
	pCtx->fn_       = _fn;
	return pCtx;
}

void CoFreeCtx(CoContext* _ctx)
{
	if (_ctx == nullptr)
		return;
	g_cCoMgr.FreeCtx(_ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 코루틴 진입점. asm이 fn_ 대신 이 함수로 점프한다.
// - fn에서 빠져나온 C++ 예외를 코루틴 스택에서 잡아 두고,
//   스케줄러 스택으로 돌아간 뒤 CoTakePendingException으로 다시 던진다.
// - 받을 사람 없는 스택 밖으로 예외가 전파되면 언와인더가 트램폴린에서
//   꼬여 프로세스가 죽으므로 여기서 반드시 끊는다.
//////////////////////////////////////////////////////////////////////////////////////////
void CoEntry(CoContext* _pCtx) noexcept
{
	// 이전 보관분이 있으면 여기서 정리한다. (take 후 버리기를 잊은 경우)
#ifdef _DEBUG
	if (tls_coPendingException)
		jc_assert_msg(false, _T("CoEntry: 꺼내지 않은 보관 예외가 버려집니다."));
#endif
	tls_coPendingException = nullptr;
	try
	{
		_pCtx->fn_(_pCtx);
	}
	catch (...)
	{
		tls_coPendingException = std::current_exception();
		tls_coLastError = coeException;
	}
	// 종료는 스위치아웃이다. 직접 스케줄러로 돌아간다.
	// - 스케줄러 쪽 래퍼가 csEnd를 보고 FreeCtx한다. 여기로 복귀하는 일은 없다.
	_pCtx->state_ = csEnd;
	CoSwitchImpl(&_pCtx->regs_, &_pCtx->callerRegs_);
	jc_assert_msg(false, _T("CoEntry: 종료 스위치에서 복귀함"));
	std::terminate();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 보관 예외 수령 + 정리
//////////////////////////////////////////////////////////////////////////////////////////
bool CoTakePendingException()
{
	if (!tls_coPendingException)
		return false;
	// 예외 자체가 보고서이므로 에러 코드는 지운다.
	tls_coLastError = coeNone;
	// 보관 소유권을 유지한 채 던진다.
	// - 풀어버리면 unwind 중 마지막 소유자가 사라져 객체가 유실되므로
	//   catch가 끝난 뒤 CoClearPendingException()으로 버릴 것.
	std::rethrow_exception(tls_coPendingException);
	return true;
}

void CoClearPendingException()
{
	tls_coPendingException = nullptr;
	tls_coLastError = coeNone;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 오버플로우 복구 + 표시.
// - 비상 밴드(미리 RW 커밋)가 있어 __except 핸들러까지는 도달한다.
//   거기서 CoNoteStackOverflow()로 표시하고, 계속 쓰려면 CoResetStackOverflow()로
//   가드존을 다시 세운다. (스택 위 객체는 망가졌을 수 있어 종료를 권장)
//////////////////////////////////////////////////////////////////////////////////////////
void CoNoteStackOverflow()
{
	CoContext* pCtx = g_cCoMgr.GetCurrentCtx();
	if (pCtx != nullptr)
		pCtx->stack_.overflowed_ = true;
}

bool CoMgr::ResetOverflow()
{
	CoContext* pCtx = currentCtx_;
	if (pCtx == nullptr || !pCtx->stack_.overflowed_)
		return false;
	CoStack& st = pCtx->stack_;

	// 현재 rsp 아래에 가드존을 다시 만들 여유가 있어야 한다.
	char* rsp = (char*)_AddressOfReturnAddress();
	char* pNewLimit = (char*)(((uintptr_t)rsp & ~(uintptr_t)(CO_PAGE_SIZE - 1)) - CO_PAGE_SIZE);
	char* pNewGuard = pNewLimit - CO_PAGE_SIZE * pageGuardCount_;
	if (pNewGuard < st.pEmergencyTop_)
		return false;

	DWORD old = 0;
	if (!::VirtualProtect(pNewGuard, CO_PAGE_SIZE * pageGuardCount_,
		PAGE_READWRITE | PAGE_GUARD, &old))
		return false;
	st.pStackLimit_ = pNewLimit;
	st.pGuardLimit_ = pNewGuard;
	st.overflowed_  = false;
	__writegsqword(0x10, (_u64)pNewLimit);
	return true;
}

bool CoResetStackOverflow()
{
	return g_cCoMgr.ResetOverflow();
}
