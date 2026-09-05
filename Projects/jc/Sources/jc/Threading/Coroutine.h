/*
 * 작성자 : 윤정도
 * 코루틴 구현 헤더
 * - 비대칭 스택 기반 (asymmetric stackful coroutine)
 * - Windows x64 전용 (MASM)
 * - 각 코루틴에 전용 스택 할당, 가드 페이지로 오버플로우 감지 + 자동 확장
 * - 컨텍스트 스위치: Windows x64 callee-saved 레지스터 전부 저장/복원
 */

#pragma once

#include "jc/Container/LinkedList.h"
#include "jc/Container/TreeMap.h"

#define CO_PAGE_SIZE			4096

#define CO_PAGE_INIT_COUNT		2		// commit
#define CO_PAGE_GUARD_COUNT		3		// commit + page guard

#define CO_STACK_SIZE_LOW		(4	* CO_PAGE_SIZE) // 16 KB 
#define CO_STACK_PAGE_COUNT_LOW	(CO_STACK_SIZE_LOW / CO_PAGE_SIZE)

#define CO_STACK_SIZE_MID		(16 * CO_PAGE_SIZE) // 64 KB (무조건 LOW의 배수로 잡을 것)
#define CO_STACK_PAGE_COUNT_MID	(CO_STACK_SIZE_MID / CO_PAGE_SIZE)

#define CO_STACK_SIZE_HIGH		(64 * CO_PAGE_SIZE)	// 256 KB (무조건 LOW의 배수로 잡을 것)
#define CO_STACK_PAGE_COUNT_HIGH (CO_STACK_SIZE_HIGH / CO_PAGE_SIZE)

#define CO_STACK_MAGIC			0x1F04210951DFBEE

// [코루틴-04] 비상 패드 페이지 수. 오버플로우 때 SEH 디스패치가 돌 공간이다.
// - 예약 하단(pStackEnd_) 아래에 미리 RW 커밋해 둔다.
// - 오버플로우 폴트 때 RSP는 예약 하단 근처라 그 아래가 비어 있으면 배달 자체가
//   죽으므로, 패드가 배달 공간을 보장한다. (스레드 스택 개런티와 같은 역할)
#define CO_PAGE_EMERGENCY_COUNT	4

// [코루틴-06] 스택 한 장이 최소 몇 페이지여야 하는지 계산한다.
// (오버플로우 가드 1 + 비상 N + 가드 + 초기 커밋)
// - 현재 cstLow(16KB=4p)는 1+0+3+2=6p를 담지 못하므로 InitStack이 가드존을
//   클램프해서 동작한다. 크기를 키우면 기존 동작이 바뀌므로 현상 유지하고,
//   티어별 상수가 필요해지면 여기서 확장한다.
constexpr _u32 CoMinStackPages(int _guard, int _init, int _emergency = 0)
{
	return 1 + (_u32)_emergency + (_u32)_guard + (_u32)_init;
}

// 스택 등급
struct CoContext;

enum CoStackTier
{
	cstNone = 0,
	cstLow,
	cstMid,
	cstHigh,
	cstCustom,

	cstReservedTierBegin	= cstLow,
	cstReservedTierEnd		= cstHigh,
	cstReservedTierCount	= cstReservedTierEnd - cstReservedTierBegin + 1,

	cstValidTierBegin		= cstLow,
	cstValidTierEnd			= cstCustom,
	cstValidTierCount		= cstValidTierEnd - cstValidTierBegin + 1,
};

enum CoState
{
	// 초기 생성 상태
	// - 스택 할당받은 상태
	csInit,

	// 실행 상태
	// - callable 함수가 실행중인 상태
	csRun,

	// 대기 상태
	// - CoYield 처리된 상태
	csYield,

	// 종료 상태
	// - 스택 반환완료
	// - 코루틴 함수 종료된 상태
	csEnd,
};

// [코루틴-07] 코루틴 실패 원인을 숫자로 구분한다.
// - 이전에는 실패해도 원인(g_cCoLastError)을 읽을 방법이 없어 CoRun이 nullptr을
//   돌려줘도 fn이 null인지 메모리 부족인지 알 수 없었으므로 공개 enum으로 바꾼다.
enum CoError : _u32
{
	coeNone = 0,
	coeNullFunction,		// fn이 nullptr
	coeVirtualAlloc,		// 스택 예약(VirtualAlloc) 실패
	coeCommitFailed,		// 스택 커밋(InitStack) 실패
	coeInvalidStackSize,	// cstCustom인데 크기가 0
	coeInvalidCtx,			// 유효하지 않은 컨텍스트
	coeWrongThread,			// 생성 스레드와 다른 스레드에서 resume
	coeInvalidState,		// resume할 수 없는 상태
	coeStaleHandle,			// 이미 종료되어 재사용된 핸들
	coeException,			// fn에서 예외가 발생해 보관 중
};

#pragma pack(push, 8)

struct CoStack
{
	// 스택 메모리 레이아웃
	// ---------------- pStackBase_  (높은 주소, 초기 RSP)
	// |			  |
	// |  COMMIT      |  ← pageInitCount_ pages
	// |			  |
	// ---------------- pStackLimit_ (일반 커밋 영역 하단 = 가드 존 상단)
	// |			  |
	// |  PAGE GUARD  |  ← pageGuardCount_ pages
	// |			  |
	// ---------------- pGuardLimit_ (가드 존 하단 = 예약 영역 상단)
	// |			  |
	// |  (reserve)   |
	// |			  |
	// ---------------- pStackEnd_   (낮은 주소)

	_u32		size_			= 0;		// 예약된 스택 사이즈 (페이지 단위로 예약)

	union
	{
		char* pStackBase_ = nullptr;// 스택의 시작 주소	= 예약 영역의 끝 주소
		char* pBaseEnd_;
	};
	
	union
	{
		char* pStackEnd_ = nullptr;	// 스택의 끝 주소		= 예약 영역의 시작 주소
		char* pBaseAddr_;
	};
	
	char*		pStackLimit_	= nullptr;	// 페이지 가드가 아닌 Commit된 영역의 끝 주소
	char*		pGuardLimit_	= nullptr;	// 페이지 가드의 끝 주소 (pGuardLimit_과 pStackLimit_이 같다면 이제 더이상 페이지가드가 없다는 뜻..)
	CoStackTier stackTier_		= cstNone;
	// [코루틴-05] 스택 식별 매직. 풀에 있거나 해제된 메모리를 가리키는
	// 잘못된 컨텍스트를 resume 전에 가려내기 위해 사용한다.
	_u64		magic_			= 0;
	// [코루틴-04] 비상 밴드 상단. 일반 확장이 내려갈 수 있는 하한이다.
	// - pStackEnd_ + PAGE 지점이다. (오버플로우 가드 1페이지 바로 위)
	// - TEB DeallocationStack에도 이 값을 설치해, 그 아래는 커널이
	//   스택 오버플로우로 확정하게 한다.
	char*		pEmergencyTop_	= nullptr;
	// [코루틴-04] 예약 시작 주소. (아래 패드 때문에 pStackEnd_와 다르다)
	// - 해제(MEM_RELEASE)는 여기서부터 해야 한다.
	char*		pReserveBase_	= nullptr;
	// [코루틴-04] 비상 밴드가 이미 오버플로우 처리에 쓰였는지 여부.
	bool		overflowed_		= false;
};

struct CoRegs
{
	_u64 rip_  = 0;		// offset  0
	_u64 rsp_  = 0;		// offset  8
	_u64 rbp_  = 0;		// offset 16

	// 스위칭전 TEB의 gs:[8]과 gs:[16]에 저장된 StackBase와 StackLimit을 보관하는 용도
	_u64 gs8_  = 0;		// offset 24  TEB StackBase:  0x8
	_u64 gs16_ = 0;		// offset 32  TEB StackLimit: 0x10
	// [코루틴-02/04] TEB DeallocationStack (TEB+0x1478) 보관용.
	// - 예약 하단이 아니라 비상 밴드 상단(pEmergencyTop_)을 설치한다.
	//   예약 하단을 설치하면 커널이 가드 폴트를 직접 확장해 VEH가 안 불리고,
	//   밴드 상단을 경계로 두면 그 아래는 커널이 스택 오버플로우로 확정한다.
	_u64 gs1478_ = 0;	// offset 40  TEB DeallocationStack: 0x1478

	// Windows x64 callee-saved 정수 레지스터
	_u64 rsi_  = 0;		// offset 48
	_u64 rdi_  = 0;		// offset 56
	_u64 r12_  = 0;		// offset 64
	_u64 r13_  = 0;		// offset 72
	_u64 r14_  = 0;		// offset 80
	_u64 r15_  = 0;		// offset 88

	// [코루틴-08] 부동소수점 제어 상태. Win x64 ABI에서 callee-saved이므로 교체한다.
	// - 코루틴이 반올림 모드를 바꾸고 yield하면 다른 코루틴까지 영향을 받는다.
	_u32 mxcsr_ = 0x1F80;	// offset  96
	_u16 fpucw_ = 0x027F;	// offset 100
	_u16 _padFp_ = 0;		// offset 102

	// Windows x64 callee-saved XMM 레지스터 (16 bytes each, 8-byte aligned)
	_u8 xmm6_[16]  = {};	// offset 104
	_u8 xmm7_[16]  = {};	// offset 120
	_u8 xmm8_[16]  = {};	// offset 136
	_u8 xmm9_[16]  = {};	// offset 152
	_u8 xmm10_[16] = {};	// offset 168
	_u8 xmm11_[16] = {};	// offset 184
	_u8 xmm12_[16] = {};	// offset 200
	_u8 xmm13_[16] = {};	// offset 216
	_u8 xmm14_[16] = {};	// offset 232
	_u8 xmm15_[16] = {};	// offset 248
	// sizeof(CoRegs) = 264
};

using FnCoroutine = void(*)(CoContext*);

struct CoContext
{
	_u32		id_ = 0;
	_u32		threadId_ = 0;
	// [코루틴-05] 세대 번호. 해제될 때마다 증가한다.
	// - 종료된 컨텍스트가 풀에 들어갔다가 다른 코루틴으로 재사용되면
	//   예전 포인터로 resume해도 엉뚱한 코루틴이 실행되는 ABA 문제가 생기므로,
	//   핸들에 세대를 함께 들고 있어 재사용을 가려낸다. (InitCtx에서 리셋하지 않음)
	_u32		generation_ = 0;
	_u32		_padGen_ = 0;
	CoRegs		regs_;
	CoStack		stack_;
	CoState		state_;
	FnCoroutine	fn_ = nullptr;
	CoContext*	callerCtx_ = nullptr;	// CoOnBeforeLaunch 진입 시 이전 currentCtx_ 보관
};

#pragma pack(pop)

// [코루틴-05] 세대가 포함된 코루틴 핸들.
// - 생 CoContext*는 종료 후 풀 재사용되면 엉뚱한 코루틴을 가리키게 되므로(ABA),
//   핸들에 세대를 함께 들고 있어 IsAlive()로 유효성을 검사한다.
struct CoHandle
{
	CoContext*	pCtx = nullptr;
	_u32		generation = 0;

	bool IsAlive() const
	{
		return pCtx != nullptr
			&& pCtx->generation_ == generation
			&& pCtx->state_ != csEnd;
	}
};

class CoMgr
{
public:
	// [코루틴-07] 첫 사용 시 CoVEH를 프로세스에 1회 자동 등록한다.
	// - 이전에는 사용자가 직접 AddVectoredExceptionHandler를 호출해야 해서
	//   등록을 빼먹으면 첫 스택 확장 시점에 가드 폴트가 처리되지 않고 종료됐다.
	CoMgr();
	// [코루틴-15] 스레드 종료 후에는 매니저를 건드리지 않는다.
	// - TLS 소멸 중/이후 가드 폴트가 오면 CoVEH가 매니저를 보지 않게 한다.
	~CoMgr();

	// [코루틴-15] TLS 생존 여부. 소멸자 시작에서 false가 된다.
	static thread_local bool t_alive_;

	// ── Context 레벨 ──────────────────────────────────────────────────────────
	// [코루틴-07] 풀에서 꺼낸 스택을 다시 커밋하다 실패할 수 있으므로 결과를 돌려준다.
	// - 이전에는 InitStack이 커밋 실패를 무시하고 진행해서 커밋 안 된 스택으로
	//   진입하다 첫 push에서 AV가 났다.
	bool		InitCtx(CoContext* _pCtx);
	CoContext*	AllocCtx(CoStackTier _stackTier, _u32 _stackSize = 0);
	void		FreeCtx(CoContext* _pCtx);

	CoContext*	FindContextByBase(char* _pBase);
	CoContext*	FindContextByAddr(char* _pAddr);
	bool		TryFindContextByBase(char* _pBase, OUT CoContext** _pOut);
	bool		TryFindContextByAddr(char* _pAddr, OUT CoContext** _pOut);

	// ── Stack 레벨 (CoStack* 직접 접근이 필요한 경우) ─────────────────────────
	CoStack*	FindStackByBase(char* _pBase);
	CoStack*	FindStackByAddr(char* _pAddr);
	bool		TryFindStackByBase(char* _pBase, OUT CoStack** _pOut);
	bool		TryFindStackByAddr(char* _pAddr, OUT CoStack** _pOut);

	// ── 공통 ──────────────────────────────────────────────────────────────────
	// [코루틴-03] 확장과 함께 TEB StackLimit을 갱신해야 해서 컨텍스트를 받는다.
	bool		ExpandStack(CoContext* _pCtx, char* _pFaultAddr);
	// [코루틴-04] 오버플로우를 잡은 뒤 가드존을 다시 세운다. (실패 시 false)
	bool		ResetOverflow();
	void		DumpStack(CoStack* _pStack, const char* _pTitle = nullptr);
	void		Clear();

	// ── 현재 실행 중인 코루틴 컨텍스트 (O(1) 접근) ───────────────────────────
	CoContext*	GetCurrentCtx() const { return currentCtx_; }
	CoContext*	currentCtx_ = nullptr;	// CoOnBeforeLaunch/AfterLaunch에서 직접 설정

	// [코루틴-15] VEH가 실제로 쓴 깊이 통계. (디버그용. 가드 예산 튜닝 근거)
	struct CoVehStats
	{
		size_t maxDispatchUsed = 0;	// 커널+ntdll이 밀어넣은 최대량
		size_t minRemain = (size_t)-1;	// 가드존 바닥까지 최소 여유
	};
	CoVehStats vehStats_;
	CoVehStats	GetVehStats() const { return vehStats_; }

	// [코루틴-05] 관리 중인(using_) 컨텍스트인지 확인한다. (Debug 검증용)
	bool		IsUsing(CoContext* _pCtx);

	// [코루틴-06] 요청(tier, size)을 실제(tier, size)로 바꾼다.
	// - 작은 custom 요청은 티어로 올리되 크기도 티어 크기로 맞춰 풀 오염을 막고,
	//   진짜 큰 custom은 페이지 단위로 올림해 풀에 넣지 않는다.
	static bool ResolveTier(CoStackTier _tier, _u32 _size, OUT CoStackTier* _pTier, OUT _u32* _pSize);

	// [코루틴-08] CET(User Shadow Stack)가 켜져 있는지 확인한다.
	static bool IsShadowStackEnabled();

	// [코루틴-04] 비상 패드 페이지 수. 전체의 1/4을 넘지 않게 한다.
	_u32		EmergencyPadPages(_u32 _totalPages) const;

	// 테스트 전용: 내부 파라미터를 외부에서 설정한다.
	void		SetPageInitCount(_u32 _count) { pageInitCount_ = _count; }
	void		SetPageGuardCount(_u32 _count) { pageGuardCount_ = _count; }
	void		SetPageGrowCount(_u32 _count) { pageGrowCount_ = _count; }
	// [코루틴-04] 비상 페이지 수. 작은 스택에 다 안 들어가면 InitStack이 클램프한다.
	void		SetPageEmergencyCount(_u32 _count) { pageEmergencyCount_ = _count; }

private:
	// [코루틴-07] 커밋 실패를 호출자에게 알리기 위해 bool을 돌려준다.
	bool		InitStack(CoStack* _pStack);
	bool		AllocStack(OUT CoStack* _pStack, CoStackTier _stackTier, _u32 _stackSize);
	void		FreeStack(CoStack* _pStack);

	_u32 pageInitCount_  = 2;
	_u32 pageGuardCount_ = 3;
	_u32 pageGrowCount_  = 2;	// 확장 시 한 번에 늘리는 페이지 수 (soft 오버플로우 방지 위해 1 이상 권장)
	// [코루틴-04] 비상 페이지 수. 오버플로우 때 SEH 디스패치가 돌 공간이다.
	_u32 pageEmergencyCount_ = CO_PAGE_EMERGENCY_COUNT;
	_u32 nextId_         = 0;

	jc::LinkedList<CoContext*>	  free_[cstReservedTierCount + 1];
	jc::TreeMap<char*, CoContext*> using_;	// key: CoStack::pStackBase_
};

extern thread_local CoMgr g_cCoMgr;


extern "C"
{
	CoContext*	CPP_CALL CoAllocCtx(FnCoroutine _fn, CoStackTier _stackTier, _u32 _stackSize);
	void		CPP_CALL CoFreeCtx(CoContext* _ctx);
	CoContext*  CPP_CALL CoCurrentCtx();
	bool		CPP_CALL CoValidateAddr(CoContext* _pCtx, char* _pAddr);

	void		CPP_CALL CoOnBeforeLaunch(CoContext* _pCtx);
	void		CPP_CALL CoOnAfterLaunch(CoContext* _pCtx);

	CoContext*  ASM_CALL CoRun(FnCoroutine _fn, CoStackTier _stackTier = cstMid, _u32 _stackSize = 0);
	void		ASM_CALL CoYieldImpl();
	CoContext*	ASM_CALL CoResume(CoContext* _pCtx);

	// [코루틴-07] 마지막 코루틴 실패 원인을 돌려주고 지운다. (스레드별)
	CoError		CPP_CALL CoGetLastError();
	// [코루틴-07] CoError를 사람이 읽을 수 있는 문자열로 변환한다.
	const char* CPP_CALL CoErrorString(CoError _err);

	// [코루틴-05] resume 전에 컨텍스트가 유효한지 검사한다.
	// - 다른 스레드, 잘못된 상태, 자기 자신이면 false를 돌려주고 원인을 남긴다.
	bool		CPP_CALL CoValidateResume(CoContext* _pCtx);
	// [코루틴-05] 핸들이 살아있는지 검사하고 resume한다. (죽은 핸들은 coeStaleHandle)
	CoContext*	CPP_CALL CoResumeH(CoHandle _h);

	// [코루틴-01] 코루틴 진입점. fn을 noexcept 경계 안에서 호출하고,
	// 빠져나온 예외는 보관해 둔다. (asm이 fn_ 대신 이 함수로 점프한다)
	void		CPP_CALL CoEntry(CoContext* _pCtx) noexcept;
	// [코루틴-01] 보관된 예외가 있으면 스케줄러 스택에서 다시 던진다. (1회성)
	// - 던지는 동안 보관 소유권을 유지해야 해서 take 뒤에도 보관분이 남는다.
	//   catch 뒤에는 CoClearPendingException()으로 버릴 것. (잊어도 다음
	//   코루틴 시작 시 정리되지만, 그 전 take는 묵은 예외를 던지므로 주의)
	bool		CPP_CALL CoTakePendingException();
	// [코루틴-01] 보관된 예외를 버린다. (catch 뒤 호출)
	void		CPP_CALL CoClearPendingException();

	// [코루틴-04] 오버플로우를 잡은 뒤 계속 쓰려면 가드존을 다시 세운다.
	// - 현재 rsp 아래에 여유가 있어야 하며, 실패하면 false. (그럼 종료할 것)
	// - 성공해도 스택 위 객체들은 이미 망가졌을 수 있어 재개보다 종료를 권장한다.
	bool		CPP_CALL CoResetStackOverflow();
	// [코루틴-04] 커널이 올린 스택 오버플로우를 잡았다고 표시한다.
	// - 커널이 직접 올린 오버플로우는 우리를 거치지 않으므로 __except 안에서
	//   호출해 overflowed_를 찍는다. (CoResetStackOverflow의 전제 조건은 아님)
	void		CPP_CALL CoNoteStackOverflow();
}

// [코루틴-07] 코루틴 밖(스레드 스택)에서 CoYield를 호출하면 asm이 null 컨텍스트를
// 역참조해 크래시나므로 진입 전에 검사한다. Debug에서는 즉시 문제를 알리고,
// Release에서는 조용히 무시하고 복귀한다.
inline void CoYield()
{
	jc_assert_msg(CoCurrentCtx() != nullptr, "CoYield: 코루틴 밖에서 호출됨");
	if (CoCurrentCtx() == nullptr)
		return;
	CoYieldImpl();
}

// [코루틴-05] 세대가 포함된 핸들로 코루틴을 시작한다.
// - CoRun이 종료까지 돌고 nullptr을 돌려준 경우(최초 yield 전 종료)는 빈 핸들이다.
inline CoHandle CoRunH(FnCoroutine _fn, CoStackTier _tier = cstMid, _u32 _size = 0)
{
	CoContext* pCtx = CoRun(_fn, _tier, _size);
	return pCtx ? CoHandle{ pCtx, pCtx->generation_ } : CoHandle{};
}

// [코루틴-01] 예외를 스케줄러 스택에서 안전하게 받는다.
// - fn이 예외를 던지면 코루틴은 종료되고 CoRun/CoResume은 nullptr을 돌려준다.
//   보관된 예외는 여기서 다시 던져진다. (받을 사람이 없는 스택 밖 전파 대신)
inline CoContext* CoRunChecked(FnCoroutine _fn, CoStackTier _tier = cstMid, _u32 _size = 0)
{
	CoContext* pCtx = CoRun(_fn, _tier, _size);
	if (pCtx == nullptr)
		CoTakePendingException();
	return pCtx;
}

// [코루틴-01] CoRunChecked의 resume 버전.
inline CoContext* CoResumeChecked(CoContext* _pCtx)
{
	CoContext* pRet = CoResume(_pCtx);
	if (pRet == nullptr)
		CoTakePendingException();
	return pRet;
}

LONG CALLBACK CoVEH(EXCEPTION_POINTERS* _pEp) noexcept;