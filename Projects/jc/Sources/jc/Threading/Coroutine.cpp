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

// [코루틴-07] 마지막 코루틴 실패 원인. (스레드별 보관)
// - 이전에는 파일 전역(g_cCoLastError)이라 사용자가 읽을 방법이 없었고,
//   스레드별로 매니저가 따로 있는데 에러는 공유라 원인 추적이 어긋났다.
static thread_local CoError t_coLastError = coeNone;

CoError CoGetLastError()
{
	// 읽으면 지운다. 같은 실패를 두 번 보고하지 않기 위함이다.
	CoError err = t_coLastError;
	t_coLastError = coeNone;
	return err;
}

const char* CoErrorString(CoError _err)
{
	switch (_err)
	{
	case coeNone:				return "성공";
	case coeNullFunction:		return "코루틴 함수가 null";
	case coeVirtualAlloc:		return "스택 예약 실패";
	case coeCommitFailed:		return "스택 커밋 실패";
	case coeInvalidStackSize:	return "잘못된 스택 크기";
	case coeInvalidCtx:			return "유효하지 않은 컨텍스트";
	case coeWrongThread:		return "생성 스레드와 다른 스레드에서 호출";
	case coeInvalidState:		return "호출할 수 없는 상태";
	case coeStaleHandle:		return "이미 종료된 핸들";
	case coeException:			return "코루틴 함수에서 예외 발생";
	default:					return "알 수 없는 오류";
	}
}

// [코루틴-07] CoVEH 자동 등록. (프로세스 1회, 가장 앞 순서)
// - 이전에는 사용자가 직접 AddVectoredExceptionHandler를 호출해야 해서
//   등록을 빼먹으면 첫 스택 확장 시점에 가드 폴트가 처리되지 않고 종료됐다.
// - Config.cpp의 전역 VEH보다 나중에 FIRST로 등록하므로 CoVEH가 먼저 불린다.
static void* s_pCoVeh = nullptr;
static std::once_flag s_coVehOnce;

static void EnsureCoVehRegistered()
{
	std::call_once(s_coVehOnce, []
	{
		s_pCoVeh = ::AddVectoredExceptionHandler(1, CoVEH);
		jc_assert_msg(s_pCoVeh != nullptr, "CoVEH 등록 실패");
	});
}

CoMgr::CoMgr()
{
	t_alive_ = true;
	// 스레드별 매니저가 처음 만들어지는 시점에 VEH를 보장한다.
	EnsureCoVehRegistered();

	// [코루틴-08] CET(User Shadow Stack)가 켜진 프로세스에서는 코루틴을 쓸 수 없다.
	// - rsp를 임의로 바꾸고 ret하는 방식이라 섀도우 스택 검사에 걸려 즉사하므로
	//   링커 /CETCOMPAT:NO로 빌드해야 한다. 여기서 미리 막는다.
	jc_assert_msg(!IsShadowStackEnabled(),
		"jc 코루틴은 CET(User Shadow Stack)와 호환되지 않는다. /CETCOMPAT:NO로 링크할 것");
}

CoMgr::~CoMgr()
{
	// [코루틴-15] 이후 가드 폴트는 모두 스레드 스택의 것으로 보고 OS에 맡긴다.
	// - Clear()에서는 내리지 않는다. Clear는 풀 비우기라 뒤에도 재사용되기 때문이다.
	t_alive_ = false;
}

thread_local bool CoMgr::t_alive_ = false;

// [코루틴-08] 섀도우 스택이 켜져 있는지 확인한다.
bool CoMgr::IsShadowStackEnabled()
{
	PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY policy{};
	if (::GetProcessMitigationPolicy(::GetCurrentProcess(),
		ProcessUserShadowStackPolicy, &policy, sizeof(policy)))
		return policy.EnableUserShadowStack != 0;
	return false;
}

// [코루틴-04] 비상 패드 페이지 수. 전체의 1/4을 넘지 않게 한다.
// - Low(4p)는 1장, Mid(16p)/High(64p)은 4장(기본값 상한)이 된다.
_u32 CoMgr::EmergencyPadPages(_u32 _totalPages) const
{
	_u32 cap = _totalPages / 4;
	return pageEmergencyCount_ < cap ? pageEmergencyCount_ : cap;
}

_u32 g_coNextId_ = 0;

thread_local CoMgr g_cCoMgr;

// [코루틴-14] CoRunU 핸드오프. CoAllocCtx가 읽는 즉시 지운다.
thread_local void* t_coStartUserData = nullptr;

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] InitStack
//   CoStack의 pStackEnd_, size_, stackTier_ 가 설정된 상태에서 호출.
//   스택 상단 pageInitCount_ 페이지 commit + pageGuardCount_ 페이지 PAGE_GUARD commit.
// [코루틴-07] 커밋 실패를 무시하지 않고 false로 돌려준다.
// - 이전에는 실패해도 void로 진행해서 커밋 안 된 스택으로 진입하다 첫 push에서 AV가 났다.
//////////////////////////////////////////////////////////////////////////////////////////
bool CoMgr::InitStack(CoStack* _pStack)
{
	_pStack->pStackBase_ = _pStack->pStackEnd_ + _pStack->size_;

	// [코루틴-04] 경계 확정.
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
			_LogError_("VirtualAlloc (init commit) failed. Error: %lu", GetLastError());
			t_coLastError = coeCommitFailed;
			return false;
		}
	}

	// guard 영역: 비상 밴드 상단 미만으로 내려가지 않도록 클램프
	char* pGuardAddr = pCommitAddr - (pageGuardCount_ * CO_PAGE_SIZE);
	if (pGuardAddr < _pStack->pEmergencyTop_)
		pGuardAddr = _pStack->pEmergencyTop_;

	// [코루틴-02] 성장 가드존은 PAGE_GUARD로 둔다.
	// - NOACCESS로 두면 커널이 예외 배달용 CONTEXT를 유저 스택에 밀어넣지 못해
	//   VEH가 호출되기도 전에 프로세스가 죽으므로 GUARD여야 한다.
	_u32 actualGuardCount = (_u32)((pCommitAddr - pGuardAddr) / CO_PAGE_SIZE);
	if (actualGuardCount > 0)
	{
		if (VirtualAlloc(pGuardAddr, actualGuardCount * CO_PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
		{
			_LogError_("VirtualAlloc (guard commit) failed. Error: %lu", GetLastError());
			t_coLastError = coeCommitFailed;
			return false;
		}
	}

	_pStack->pStackLimit_ = pCommitAddr;
	_pStack->pGuardLimit_ = pGuardAddr;
	// [코루틴-12] 커밋 하한 기록. 풀 반납 때 이 위로만 유지한다.
	_pStack->pCommitLow_ = pGuardAddr;

	// [코루틴-04] 오버플로우 가드 1페이지를 GUARD로 둔다. (절대 해제 금지)
	// - NOACCESS로 두면 RSP가 바로 위에 있을 때 배달 push가 실패해 죽으므로
	//   GUARD로 둔다. (배달 push가 가드를 자동 해제한다)
	if (VirtualAlloc(_pStack->pStackEnd_, CO_PAGE_SIZE,
		MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD) == nullptr)
	{
		_LogError_("VirtualAlloc (overflow guard commit) failed. Error: %lu", GetLastError());
		t_coLastError = coeCommitFailed;
		return false;
	}

	// [코루틴-04] 비상 패드(예약 아래 N페이지)를 RW 커밋한다.
	// - 패드 크기는 예약 때 정해진다. (pStackEnd_ - pReserveBase_)
	if (_pStack->pReserveBase_ < _pStack->pStackEnd_)
	{
		if (VirtualAlloc(_pStack->pReserveBase_,
			(SIZE_T)(_pStack->pStackEnd_ - _pStack->pReserveBase_),
			MEM_COMMIT, PAGE_READWRITE) == nullptr)
		{
			_LogError_("VirtualAlloc (emergency pad commit) failed. Error: %lu", GetLastError());
			t_coLastError = coeCommitFailed;
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [Private] AllocStack
//   확정된 tier/size로 메모리를 예약(RESERVE)하고 InitStack을 호출.
//   티어 판별은 하지 않는다. (호출 전 ResolveTier로 확정할 것)
// [코루틴-06] 이전에는 여기서 티어만 바꾸고 크기는 그대로 둬서
//   5000B짜리 스택이 cstLow 풀로 들어가 다음 사용자를 오염시켰다.
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

	// ResolveTier를 거친 크기이므로 페이지 정렬이어야 한다.
	jc_assert_msg((_stackSize & (CO_PAGE_SIZE - 1)) == 0,
		"스택 크기가 페이지 정렬이 아닙니다. size: %u", _stackSize);
	if ((_stackSize & (CO_PAGE_SIZE - 1)) != 0)
	{
		t_coLastError = coeInvalidStackSize;
		return false;
	}

	// [코루틴-04] 예약 아래에 비상 패드를 덧붙인다. (오버플로우 배달 공간)
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
			t_coLastError = coeVirtualAlloc;
			return false;
		}
	}
	else
	{
		// [코루틴-13] 슬랩에서 슬롯을 잘라 쓴다. (64KB 그래뉴러리티 낭비 제거)
		// - 1MB 슬랩을 티어 크기로 분할. 빈 슬랩이 없으면 새로 예약한다.
		// - 반납된 슬롯 재사용은 free_ 풀 몫이라 used는 앞으로만 간다.
		StackSlab* pSlab = nullptr;
		if (!slabs_[_stackTier].IsEmpty())
		{
			StackSlab& back = slabs_[_stackTier].Back();
			if (back.used < back.slotCount)
				pSlab = &back;
		}
		if (pSlab == nullptr)
		{
			char* pSlabBase = (char*)VirtualAlloc(nullptr, SLAB_BYTES, MEM_RESERVE, PAGE_NOACCESS);
			if (pSlabBase == nullptr)
			{
				t_coLastError = coeVirtualAlloc;
				return false;
			}
			StackSlab slab;
			slab.pBase = pSlabBase;
			slab.slotBytes = slotBytes;
			slab.slotCount = SLAB_BYTES / slotBytes;
			slab.used = 0;
			slabs_[_stackTier].PushBack(slab);
			pSlab = &slabs_[_stackTier].Back();
		}
		pSlot = pSlab->pBase + (size_t)pSlab->slotBytes * pSlab->used;
		pSlab->used++;
	}

	_pStack->pReserveBase_ = pSlot;
	_pStack->pStackEnd_  = pSlot + padPages * CO_PAGE_SIZE;
	_pStack->size_       = _stackSize;
	_pStack->stackTier_  = _stackTier;
	// [코루틴-05] 스택 식별 매직을 찍는다. 해제된 메모리를 가리키는
	// 잘못된 컨텍스트를 resume 전에 가려내기 위해 사용한다.
	_pStack->magic_      = CO_STACK_MAGIC;
	return InitStack(_pStack);
}

//////////////////////////////////////////////////////////////////////////////////////////
// [코루틴-06] 티어/크기 결정을 한 곳으로 모은다.
// - 이전에는 AllocStack과 AllocCtx가 각자 판별 로직을 복사해 가져서 어긋날 수 있었고,
//   티어는 올리면서 크기는 그대로 둬 풀 오염과 정렬 깨짐이 생겼다.
//////////////////////////////////////////////////////////////////////////////////////////
static _u32 RoundUpPage(_u32 _v)
{
	return (_v + CO_PAGE_SIZE - 1) & ~(_u32)(CO_PAGE_SIZE - 1);
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
			t_coLastError = coeInvalidStackSize;
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
		jc_assert_msg(false, "잘못된 스택 티어입니다. tier: %d", _tier);
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
		// [코루틴-04] 패드까지 포함해 예약 전체를 해제한다.
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
// [코루틴-12] 풀 반납 시 커밋 유지 + 가드존 재배치.
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
	char* pInitLow  = _pStack->pStackBase_ - initBytes;
	char* pGuardLow = pInitLow - guardBytes;
	if (pGuardLow < _pStack->pEmergencyTop_)
		pGuardLow = _pStack->pEmergencyTop_;

	// [코루틴-12] 실제 가드존 크기. 클램프되면 guardBytes보다 작다.
	// - 클램프 무시하고 guardBytes로 Protect하면 init 영역까지 가드가 번진다.
	SIZE_T guardZoneBytes = (SIZE_T)(pInitLow - pGuardLow);

	// 유지 상한을 넘는 커밋만 디커밋한다. (보통은 0바이트)
	char* pKeepLow = _pStack->pStackBase_ - poolKeepBytes_;
	if (_pStack->pCommitLow_ != nullptr && _pStack->pCommitLow_ < pKeepLow)
	{
		VirtualFree(_pStack->pCommitLow_, (SIZE_T)(pKeepLow - _pStack->pCommitLow_), MEM_DECOMMIT);
		_pStack->pCommitLow_ = pKeepLow;
	}

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
	jc_assert_msg(_pStack->pGuardLimit_ == pGuardLow, "가드존 재설치 실패");
}

//////////////////////////////////////////////////////////////////////////////////////////
// InitCtx
//   풀에서 꺼낸 CoContext를 가볍게 리셋. 스택은 반납 때 이미 다음 사용 준비가
//   끝나서 커밋을 다시 안 한다. ([코루틴-12] 이전에는 여기서 재커밋했다)
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::InitCtx(CoContext* _pCtx)
{
	_pCtx->id_         = 0;
	_pCtx->threadId_   = 0;
	_pCtx->regs_       = {};
	_pCtx->schedRegs_  = {};
	_pCtx->state_      = csInit;
	_pCtx->fn_         = nullptr;
	_pCtx->callerCtx_  = nullptr;
	// [코루틴-14] 사용자 채널은 재사용 때마다 비운다. (세대는 유지)
	_pCtx->userData_        = nullptr;
	_pCtx->transfer_        = 0;
	_pCtx->cancelRequested_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// AllocCtx
//   풀에 재사용 가능한 CoContext가 있으면 꺼내서 InitCtx 후 반환.
//   없으면 새로 dbg_new 하고 AllocStack → stacksByBase_ 등록 후 반환.
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::AllocCtx(CoStackTier _stackTier, _u32 _stackSize)
{
	// [코루틴-06] 티어/크기 판별을 한 곳에서 확정한다.
	CoStackTier tier = cstNone;
	_u32 size = 0;
	if (!ResolveTier(_stackTier, _stackSize, &tier, &size))
		return nullptr;

	CoContext* pCtx = nullptr;

	if (tier != cstCustom && free_[tier].PopFront(&pCtx))
	{
		// [코루틴-12] 풀 재사용: 커밋 유지됨. 레지스터/상태만 리셋하고 바로 쓴다.
		// [코루틴-06] 풀 무결성 확인. 크기가 다르면 오염된 것이므로 쓰지 않는다.
		jc_assert_msg(pCtx->stack_.size_ == size,
			"풀 오염: 티어 크기와 다릅니다. tier: %d, size: %u", tier, pCtx->stack_.size_);
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

	// [코루틴-10] movaps 전제. CoContext가 16 정렬이어야 regs_ 안 XMM도 정렬된다.
	jc_assert_msg((((uintptr_t)pCtx & 15) == 0),
		"CoContext가 16 정렬이 아닙니다. pCtx: 0x%p", pCtx);

	pCtx->id_ = ++nextId_;
	stacksByBase_.Insert(pCtx->stack_.pStackBase_, pCtx);
	return pCtx;
}

//////////////////////////////////////////////////////////////////////////////////////////
// FreeCtx
//   stacksByBase_ 에서 제거 후 커스텀이면 메모리 해제 + delete,
//   풀 티어면 decommit 후 free_ 풀로 반환.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::FreeCtx(CoContext* _pCtx)
{
	jc_assert(_pCtx != nullptr);

	// [코루틴-15] 실행 중인 컨텍스트를 풀로 되돌리는 건 버그다.
	jc_assert(currentCtx_ != _pCtx);

	CoStackTier stackTier = _pCtx->stack_.stackTier_;
	jc_assert_msg(stackTier >= cstValidTierBegin && stackTier <= cstValidTierEnd,
		"잘못된 스택 티어입니다. tier: %d", stackTier);

	CoContext* pPopped = nullptr;
	if (stacksByBase_.TryPop(_pCtx->stack_.pStackBase_, &pPopped) == false)
	{
		jc_assert_msg(false, "해당 컨텍스트는 관리 중인 컨텍스트가 아닙니다. pStackBase_: 0x%p",
			_pCtx->stack_.pStackBase_);
		return;
	}

	// [코루틴-05] 해제 표시. 세대를 올려 예전 핸들을 모두 무효화한다.
	// - 종료된 컨텍스트가 풀에 들어갔다가 재사용되면 예전 포인터로
	//   엉뚱한 코루틴이 실행되는 ABA 문제가 생기므로 세대로 가려낸다.
	pPopped->generation_++;
	pPopped->state_ = csEnd;
	pPopped->fn_    = nullptr;

	if (stackTier == cstCustom)
	{
		FreeStack(&pPopped->stack_);
		delete pPopped;
	}
	else if (free_[stackTier].Size() >= (int)poolMax_[stackTier])
	{
		// [코루틴-12/13] 풀이 가득 찼다. 슬롯은 슬랩 조각이라 release하면 안 되고
		// decommit 후 버린다. (슬랩 자체는 Clear 때 해제)
		FreeStack(&pPopped->stack_);
		delete pPopped;
	}
	else
	{
		// [코루틴-12] 커밋을 유지한 채 풀로 되돌린다. 다음 AllocCtx는 커널을 안 탄다.
		RecycleStack(&pPopped->stack_);
		free_[stackTier].PushBack(pPopped);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Clear
//   stacksByBase_ 에 남은 항목 경고 후 free_ 풀 전체 해제.
//////////////////////////////////////////////////////////////////////////////////////////
void CoMgr::Clear()
{
	currentCtx_ = nullptr;

	if (stacksByBase_.Size() > 0)
	{
		jc_assert_msg(false, "Clear 호출 시점에 아직 할당된 컨텍스트가 존재합니다. Count: %zu", stacksByBase_.Size());
	}

	for (int tier = cstReservedTierBegin; tier <= cstReservedTierEnd; ++tier)
	{
		CoContext* pCtx = nullptr;
		while (free_[tier].PopFront(&pCtx))
		{
			// [코루틴-13] 풀 ctx는 슬랩 슬롯이라 개별 release 금지. delete만 한다.
			// (custom은 풀에 안 들어가서 여기 올 일이 없음)
			delete pCtx;
		}
	}

	// [코루틴-13] 슬랩 예약 전체를 해제한다.
	for (int tier = cstReservedTierBegin; tier <= cstReservedTierEnd; ++tier)
	{
		StackSlab slab;
		while (slabs_[tier].PopFront(&slab))
			VirtualFree(slab.pBase, 0, MEM_RELEASE);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// FindContextByBase / FindContextByAddr
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoMgr::FindContextByBase(char* _pBase)
{
	CoContext** pFound = stacksByBase_.Find(_pBase);
	if (pFound == nullptr)
		return nullptr;
	return *pFound;
}

CoContext* CoMgr::FindContextByAddr(char* _pAddr)
{
	CoContext** pFound = stacksByBase_.LowerBoundValue(_pAddr);
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
	CoContext** pFound = stacksByBase_.Find(_pBase);
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

// [코루틴-05] 관리 중인(stacksByBase_) 컨텍스트인지 확인한다. (Debug 검증용)
bool CoMgr::IsUsing(CoContext* _pCtx)
{
	if (_pCtx == nullptr)
		return false;
	CoContext** pFound = stacksByBase_.Find(_pCtx->stack_.pStackBase_);
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
// [코루틴-03] 확장이 끝나면 TEB StackLimit도 함께 내린다.
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

	// [코루틴-04] 비상 밴드 상단이 일반 확장이 내려갈 수 있는 하한이다.
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

		// [코루틴-04] 비상 밴드 위로 클램프 (밴드는 확장 대상이 아님)
		if (pGrowthStart < _pStack->pEmergencyTop_)
			pGrowthStart = _pStack->pEmergencyTop_;

		commitPageCount += (_u32)((pGrowthEnd - pGrowthStart) / CO_PAGE_SIZE);
	}

	if (VirtualAlloc(pGrowthStart, CO_PAGE_SIZE * commitPageCount, MEM_COMMIT, PAGE_READWRITE) == nullptr)
		return false;

	_pStack->pStackLimit_  = pGrowthStart;
	char* pNewGuardEnd     = pGrowthStart;
	char* pNewGuardStart   = pGrowthStart - pageGuardCount_ * CO_PAGE_SIZE;

	// [코루틴-04] 새 가드존도 비상 밴드 위까지만 둔다.
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

	// [코루틴-12] 커밋 하한 갱신. (새 가드존까지 커밋됨)
	if (_pStack->pCommitLow_ == nullptr || pNewGuardStart < _pStack->pCommitLow_)
		_pStack->pCommitLow_ = pNewGuardStart;

	// [코루틴-03] 이 코루틴이 지금 실행 중(폴트를 낸 스택)이면 TEB도 함께 내린다.
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
// [코루틴-15] VEH 재진입 깊이. 어떤 경로로 나가도 복구되게 RAII로 센다.
static thread_local _u32 t_coVehDepth = 0;

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

// [코루틴-15] VEH 경로 함수는 스택을 적게 쓴다. 보안 쿠키 검사가 들어갈
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
	if (!CoMgr::t_alive_)
		return EXCEPTION_CONTINUE_SEARCH;

	char* pFaultAddr = (char*)pRec->ExceptionInformation[1];
	char* pFaultRsp  = (char*)_pEp->ContextRecord->Rsp;

#ifdef _DEBUG
	// [코루틴-15] VEH가 실제로 쓴 깊이를 잰다. (가드 예산 튜닝 근거)
	{
		char* pVehRsp = (char*)_AddressOfReturnAddress();
		char* pFaultRspDbg = (char*)_pEp->ContextRecord->Rsp;
		CoMgr& mgrDbg = g_cCoMgr;
		size_t dispatchUsed = (size_t)(pFaultRspDbg - pVehRsp);
		CoContext* pDbg = mgrDbg.GetCurrentCtx();
		if (pDbg != nullptr)
		{
			size_t remain = (size_t)(pVehRsp - pDbg->stack_.pGuardLimit_);
			if (dispatchUsed > mgrDbg.vehStats_.maxDispatchUsed)
				mgrDbg.vehStats_.maxDispatchUsed = dispatchUsed;
			if (remain < mgrDbg.vehStats_.minRemain)
				mgrDbg.vehStats_.minRemain = remain;
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
	CoVehScope scope(t_coVehDepth);
	if (t_coVehDepth > 1)
	{
		if (t_coVehDepth > 2)
			return CoConvertToOverflow(_pEp, pCtx);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	// 5. 확장. 실패하면 오버플로우로 확정한다. (조용히 넘기면 다음 페이지에서 AV가 난다)
	// [코루틴-03] TEB StackLimit 갱신은 ExpandStack 안에서 한다.
	if (g_cCoMgr.ExpandStack(pCtx, pFaultAddr))
		return EXCEPTION_CONTINUE_EXECUTION;
	return CoConvertToOverflow(_pEp, pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// currentCtx_ 체인 관리 (CoOnBeforeLaunch/AfterLaunch 역할)
//   [코루틴-11] 스위치 앞뒤 처리가 C++ 인라인 래퍼로 옮겨서 전용 함수는 삭제.
//   currentCtx_/callerCtx_ 갱신은 헤더의 CoRun/CoResume/CoYield 래퍼에서 직접 한다.
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
// [코루틴-15] 스레드 스택으로 돌아온 뒤 가드존이 살아있는지 확인한다.
// - 가드 비트까지는 보지 않는다. 커널이 성장 과정에서 우리 가드를 조용히
//   커밋해 버리기 때문이다. (실측) 커밋 자체가 풀렸으면 풀 관리 버그다.
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
// [코루틴-05] CoResume 안전장치.
// - 이전에는 null 검사만 하고 레지스터를 복원했으므로 다른 스레드에서 resume하거나
//   이미 종료된 컨텍스트를 resume하면 정의되지 않은 곳으로 점프했다.
// - 여기서 걸리면 nullptr이 반환되고 원인은 CoGetLastError()로 확인한다.
//////////////////////////////////////////////////////////////////////////////////////////
bool CoValidateResume(CoContext* _pCtx)
{
	if (_pCtx == nullptr)
	{
		t_coLastError = coeInvalidCtx;
		return false;
	}
	if (_pCtx->stack_.magic_ != CO_STACK_MAGIC)
	{
		t_coLastError = coeInvalidCtx;
		return false;
	}
	if (_pCtx->threadId_ != GetCurrentThreadId())
	{
		t_coLastError = coeWrongThread;
		return false;
	}
	if (_pCtx->state_ != csInit && _pCtx->state_ != csYield)
	{
		t_coLastError = coeInvalidState;
		return false;
	}
	if (_pCtx == g_cCoMgr.GetCurrentCtx())
	{
		// 자기 자신을 resume하면 자기 스택으로 재진입해 스택이 오염된다.
		t_coLastError = coeInvalidState;
		return false;
	}
#ifdef _DEBUG
	if (!g_cCoMgr.IsUsing(_pCtx))
	{
		t_coLastError = coeInvalidCtx;
		return false;
	}
#endif
	return true;
}

CoContext* CoResumeH(CoHandle _h)
{
	if (!_h.IsAlive())
	{
		t_coLastError = coeStaleHandle;
		return nullptr;
	}
	return CoResume(_h.pCtx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CoAllocCtx / CoFreeCtx  (extern "C" 래퍼)
//////////////////////////////////////////////////////////////////////////////////////////
CoContext* CoAllocCtx(FnCoroutine _fn, CoStackTier _stackTier, _u32 _stackSize)
{
	if (_fn == nullptr)
	{
		// [코루틴-07] 실패 원인을 공개 에러 코드로 남긴다.
		t_coLastError = coeNullFunction;
		return nullptr;
	}

	CoContext* pCtx = g_cCoMgr.AllocCtx(_stackTier, _stackSize);
	if (pCtx == nullptr)
		return nullptr;

	pCtx->threadId_ = GetCurrentThreadId();
	pCtx->state_    = csInit;
	pCtx->fn_       = _fn;
	// [코루틴-14] CoRunU 핸드오프를 소비한다. (1회성. plain CoRun이면 null)
	pCtx->userData_ = t_coStartUserData;
	t_coStartUserData = nullptr;
	return pCtx;
}

void CoFreeCtx(CoContext* _ctx)
{
	if (_ctx == nullptr)
		return;
	g_cCoMgr.FreeCtx(_ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////
// [코루틴-01] 코루틴 진입점. asm이 fn_ 대신 이 함수로 점프한다.
// - fn에서 빠져나온 C++ 예외를 코루틴 스택에서 잡아 두고,
//   스케줄러 스택으로 돌아간 뒤 CoTakePendingException으로 다시 던진다.
// - 받을 사람 없는 스택 밖으로 예외가 전파되면 언와인더가 트램폴린에서
//   꼬여 프로세스가 죽으므로(또는 이상 종료) 여기서 반드시 끊는다.
//////////////////////////////////////////////////////////////////////////////////////////
static thread_local std::exception_ptr t_coPendingException;

void CoEntry(CoContext* _pCtx) noexcept
{
	// 이전 보관분이 있으면 여기서 정리한다. (take 후 버리기를 잊은 경우)
	// - 캡처와 take 사이에 사용자 코드가 끼지 않으므로 여기서 버려도 안전하다.
	t_coPendingException = nullptr;
	try
	{
		_pCtx->fn_(_pCtx);
	}
	catch (...)
	{
		t_coPendingException = std::current_exception();
		t_coLastError = coeException;
	}
	// [코루틴-11] 종료는 스위치아웃이다. 트램폴린 대신 직접 스케줄러로 돌아간다.
	// - 스케줄러 쪽 래퍼가 csEnd를 보고 FreeCtx한다. 여기로 복귀하는 일은 없다.
	_pCtx->state_ = csEnd;
	CoSwitchImpl(&_pCtx->regs_, &_pCtx->schedRegs_);
	jc_assert_msg(false, "CoEntry: 종료 스위치에서 복귀함");
	std::terminate();
}

bool CoTakePendingException()
{
	if (!t_coPendingException)
		return false;
	// 예외 자체가 보고서이므로 에러 코드는 지운다.
	t_coLastError = coeNone;
	// 보관 소유권을 유지한 채 던진다.
	// - 풀어버리면 unwind 중 마지막 소유자가 사라져 객체가 유실되므로
	//   catch가 끝난 뒤 CoClearPendingException()으로 버릴 것.
	std::rethrow_exception(t_coPendingException);
	return true;
}

void CoClearPendingException()
{
	t_coPendingException = nullptr;
	t_coLastError = coeNone;
}

//////////////////////////////////////////////////////////////////////////////////////////
// [코루틴-04] 오버플로우 복구 + 표시.
// - 이전에는 오버플로우 가드를 치는 순간 디스패치 공간이 없어 이중 폴트로
//   프로세스가 강제 종료됐고, 잡을 방법도 되돌릴 방법도 없었다.
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