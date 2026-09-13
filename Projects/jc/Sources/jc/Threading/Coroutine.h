/*
 * 작성자 : 윤정도
 * 코루틴 구현 헤더
 * - 비대칭 스택 기반 (asymmetric stackful coroutine)
 * - Windows x64 전용 (MASM)
 * - 각 코루틴에 전용 스택 할당, 가드 페이지로 오버플로우 감지 + 자동 확장
 * - 컨텍스트 스위치: Windows x64 callee-saved 레지스터 전부 저장/복원
 */

#pragma once

#include "jc/Container/HashMap.h"
#include "jc/Container/LinkedList.h"
#include "jc/Container/TreeMap.h"
#include "jc/Primitives/String.h"

#include <type_traits>
#include <utility>
#include <cstddef>
#include <exception>
#include <intrin.h>

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

// 비상 패드 페이지 수. 오버플로우 때 SEH 디스패치가 돌 공간이다.
// - 예약 하단(pStackEnd_) 아래에 미리 RW 커밋해 둔다.
// - 오버플로우 폴트 때 RSP는 예약 하단 근처라 그 아래가 비어 있으면 배달 자체가
//   죽으므로, 패드가 배달 공간을 보장한다.
#define CO_PAGE_EMERGENCY_COUNT	4

// 스택 한 장이 최소 몇 페이지여야 하는지 계산한다.
// (오버플로우 가드 1 + 비상 N + 가드 + 초기 커밋)
// - 현재 cstLow(16KB=4p)는 1+0+3+2=6p를 담지 못하므로 InitStack이 가드존을
//   클램프해서 동작한다. 크기를 키우면 기존 동작이 바뀌므로 현상 유지하고,
//   티어별 상수가 필요해지면 여기서 확장한다.
constexpr _u32 CoMinStackPages(_s32 _guard, _s32 _init, _s32 _emergency = 0)
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

enum CoError : _u32
{
	coeNone = 0,
	coeNullFunction,		// fn이 nullptr
	coeVirtualAlloc,		// 스택 예약(VirtualAlloc) 실패
	coeCommitFailed,		// 스택 커밋(InitStack) 실패
	coeInvalidStackSize,	// cstCustom인데 크기가 0
	coeInvalidCtx,			// 유효하지 않은 컨텍스트
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
	_u64		magic_			= 0;
	// 비상 밴드 상단. 일반 확장이 내려갈 수 있는 하한이다.
	// - pStackEnd_ + PAGE 지점이다. (오버플로우 가드 1페이지 바로 위)
	// - TEB DeallocationStack에도 이 값을 설치해, 그 아래는 커널이
	//   스택 오버플로우로 확정하게 한다.
	char*		pEmergencyTop_	= nullptr;
	// 예약 시작 주소. (아래 패드 때문에 pStackEnd_와 다르다)
	// - 해제(MEM_RELEASE)는 여기서부터 해야 한다.
	char*		pReserveBase_	= nullptr;
	// 비상 밴드가 이미 오버플로우 처리에 쓰였는지 여부.
	bool		overflowed_		= false;
	// 실제 커밋된 가장 낮은 주소. 풀 반납 때 이 위로만 유지하고
	// 나머지는 디커밋한다. (다음 사용자가 바로 쓰는 high-water mark)
	char*		pCommitLow_		= nullptr;
};

struct CoRegs
{
	_u64 rip_  = 0;		// offset  0
	_u64 rsp_  = 0;		// offset  8
	_u64 rbp_  = 0;		// offset 16

	// 스위칭전 TEB의 gs:[8]과 gs:[16]에 저장된 StackBase와 StackLimit을 보관하는 용도
	_u64 gs8_  = 0;		// offset 24  TEB StackBase:  0x8
	_u64 gs16_ = 0;		// offset 32  TEB StackLimit: 0x10
	// TEB DeallocationStack (TEB+0x1478) 보관용.
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

	// 부동소수점 제어 상태. Win x64 ABI에서 callee-saved이므로 교체한다.
	// - 코루틴이 반올림 모드를 바꾸고 yield하면 다른 코루틴까지 영향을 받는다.
	_u32 mxcsr_ = 0x1F80;	// offset  96
	_u16 fpucw_ = 0x027F;	// offset 100
	_u16 padFp_ = 0;		// offset 102
	// XMM 16 정렬 패딩. movaps는 정렬 위반 시 즉시 fault라
	// 정렬 오류를 바로 드러낸다.
	_u8 padAlign_[8] = {};	// offset 104

	// Windows x64 callee-saved XMM 레지스터 (16 bytes each, 16-byte aligned)
	_u8 xmm6_[16]  = {};	// offset 112
	_u8 xmm7_[16]  = {};	// offset 128
	_u8 xmm8_[16]  = {};	// offset 144
	_u8 xmm9_[16]  = {};	// offset 160
	_u8 xmm10_[16] = {};	// offset 176
	_u8 xmm11_[16] = {};	// offset 192
	_u8 xmm12_[16] = {};	// offset 208
	_u8 xmm13_[16] = {};	// offset 224
	_u8 xmm14_[16] = {};	// offset 240
	_u8 xmm15_[16] = {};	// offset 256
	// sizeof(CoRegs) = 272
};

using FnCoroutine = void(*)(CoContext*);

// 코루틴 식별자. 단조 증가하며 재사용하지 않는다. 0은 무효 id다.
using CoId = _u64;
constexpr CoId CO_INVALID_ID = 0;

// 예외 발생 시 호출될 콜백. 스케줄러 스택에서 호출된다.
// - _ex는 정확히 1회 전달된다. (move. 핸들러 복귀 후 보관에 남지 않음)
using FnCoException = void(*)(CoId _id, std::exception_ptr _ex);

struct CoContext
{
	_u64		id_ = 0;
	_u64		pad0_ = 0;	// regs_ 16 정렬 유지용
	CoRegs		regs_;
	CoStack		stack_;
	CoState		state_;
	FnCoroutine	fn_ = nullptr;
	CoContext*	pCallerCtx_ = nullptr;	// 진입 시 이전 currentCtx_ 보관
	// 사용자 컨텍스트.
	//
	// 코루틴이 시작할 때 한 번 전달하고, 끝날 때까지 그대로 유지되는 값이다.
	// CoDesc.userData_로 건네며, 코루틴 함수는 첫 줄부터 읽을 수 있다.
	// yield로 양보했다가 돌아와도 값이 그대로이므로,
	// 진행 상태와 함께 쓰면 전역 변수 없이 데이터를 주고받을 수 있다.
	//
	// 예시:
	//   _s32 acc = 0;
	//   CoId id = CoRun(Worker, {.userData_ = &acc});
	void*		userData_ = nullptr;
	// 스위치 왕복 값.
	//
	// yield와 resume이 값을 주고받는 8바이트 우체통이다.
	// 스위치가 일어날 때마다 덮어쓰므로, 오래 보관할 값은 userData_에 둔다.
	//
	// - 코루틴 쪽: CoYield(내보낼 값)가 값을 넣고 양보한다.
	//   복귀하면 CoYield의 반환값이 다음 resume 때 들어온 값이 된다.
	// - 스케줄러 쪽: CoResume(id, 넣을 값, &꺼낼 값)가 값을 넣고 재개한다.
	//   복귀하면 꺼낼 값에 코루틴이 마지막에 낸 값이 담긴다.
	//
	// 예시:
	//   void Gen(CoContext*)
	//   {
	//       _u64 request = CoYield((_u64)100);
	//       ...
	//   }
	//   _u64 out = 0;
	//   bool alive = CoResume(id, (_u64)111, &out); // out에는 100이 담긴다.
	_u64		switchData_ = 0;
	// 호출자 쪽 레지스터 세트. 스위치할 때 현재 호출자 상태가
	// 여기로 저장된다. (코루틴 상태는 regs_에 따로 저장. 공유하지 않음)
	CoRegs		callerRegs_;
	// 협력적 취소 요청. CoScope가 세운다. fn은 CoCancelRequested()로 확인한다.
	bool		isCancelRequested_ = false;
	_u8			padCancel_[7] = {};
	// 예외 발생 시 호출될 콜백. (CoDesc에서 전달. 없으면 전역 핸들러 → 보관 순)
	FnCoException onException_ = nullptr;
};

#pragma pack(pop)

// asm 구조체와 C++ 구조체의 오프셋 일치를 강제한다.
// - 필드 추가 때 어긋나기 쉬우므로 여기서 멈춘다. (asm 쪽은 수동 동기화)
static_assert(offsetof(CoRegs, rip_) == 0);
static_assert(offsetof(CoRegs, gs1478_) == 40);
static_assert(offsetof(CoRegs, rsi_) == 48);
static_assert(offsetof(CoRegs, mxcsr_) == 96);
static_assert(offsetof(CoRegs, xmm6_) == 112);
static_assert(offsetof(CoRegs, xmm6_) % 16 == 0);	// movaps 전제
static_assert(sizeof(CoRegs) == 272);

static_assert(offsetof(CoStack, pStackBase_) == 8);
static_assert(offsetof(CoStack, pStackEnd_) == 16);
static_assert(offsetof(CoStack, magic_) == 48);
static_assert(offsetof(CoStack, pEmergencyTop_) == 56);
static_assert(offsetof(CoStack, pReserveBase_) == 64);
static_assert(sizeof(CoStack) == 88);
static_assert(offsetof(CoContext, regs_) == 16);
static_assert(offsetof(CoContext, stack_) == 288);
static_assert(offsetof(CoContext, state_) == 376);
static_assert(offsetof(CoContext, fn_) == 384);
static_assert(offsetof(CoContext, userData_) == 400);
static_assert(offsetof(CoContext, callerRegs_) == 416);
static_assert(offsetof(CoContext, callerRegs_) % 16 == 0);	// movaps 전제
static_assert(offsetof(CoContext, isCancelRequested_) == 688);
static_assert(offsetof(CoContext, onException_) == 696);
static_assert(sizeof(CoContext) == 704);

class CoMgr
{
public:
	CoMgr();
	// 스레드 종료 후에는 매니저를 건드리지 않는다.
	// - TLS 소멸 중/이후 가드 폴트가 오면 CoVEH가 매니저를 보지 않게 한다.
	~CoMgr();

	// TLS 생존 여부. 소멸자 시작에서 false가 된다.
	static thread_local bool tls_alive_;

	// ── Context 레벨 ──────────────────────────────────────────────────────────
	// 풀에서 꺼낸 건 레지스터/상태만 리셋한다. 스택은 반납 때
	// 이미 다음 사용 준비가 끝나서 커밋을 다시 안 한다.
	void		InitCtx(CoContext* _pCtx);
	CoContext*	AllocCtx(CoStackTier _stackTier, _u32 _stackSize = 0);
	void		FreeCtx(CoContext* _pCtx);

	CoContext*	FindContextByBase(char* _pBase);
	CoContext*	FindContextByAddr(char* _pAddr);
	CoContext*	FindContextById(CoId _id);
	bool		TryFindContextByBase(char* _pBase, OUT CoContext** _pOut);
	bool		TryFindContextByAddr(char* _pAddr, OUT CoContext** _pOut);
	bool		TryFindContextById(CoId _id, OUT CoContext** _pOut);

	// ── Stack 레벨 (CoStack* 직접 접근이 필요한 경우) ─────────────────────────
	CoStack*	FindStackByBase(char* _pBase);
	CoStack*	FindStackByAddr(char* _pAddr);
	bool		TryFindStackByBase(char* _pBase, OUT CoStack** _pOut);
	bool		TryFindStackByAddr(char* _pAddr, OUT CoStack** _pOut);

	// ── 공통 ──────────────────────────────────────────────────────────────────
	// 확장과 함께 TEB StackLimit을 갱신해야 해서 컨텍스트를 받는다.
	bool		ExpandStack(CoContext* _pCtx, char* _pFaultAddr);
	// 오버플로우를 잡은 뒤 가드존을 다시 세운다. (실패 시 false)
	bool		ResetOverflow();
	void		DumpStack(CoStack* _pStack, const _char* _pTitle = nullptr);
	void		Clear();

	// ── 현재 실행 중인 코루틴 컨텍스트 (O(1) 접근) ───────────────────────────
	CoContext*	GetCurrentCtx() const { return currentCtx_; }
	CoContext*	currentCtx_ = nullptr;	// CoOnBeforeLaunch/AfterLaunch에서 직접 설정

	// VEH가 실제로 쓴 깊이 통계. (디버그용. 가드 예산 튜닝 근거)
	struct CoVehStats
	{
		size_t maxDispatchUsed_ = 0;	// 커널+ntdll이 밀어넣은 최대량
		size_t minRemain_ = (size_t)-1;	// 가드존 바닥까지 최소 여유
	};
	CoVehStats vehStats_;
	CoVehStats GetVehStats() const { return vehStats_; }

#ifdef _DEBUG
	// 스위치 복귀 시 가드존 커밋 검증.
	static void	VerifyGuardZone(const CoStack& _stack) noexcept;
#endif
	bool		IsUsing(CoContext* _pCtx);

	// 요청(tier, size)을 실제(tier, size)로 바꾼다.
	// - 작은 custom 요청은 티어로 올리되 크기도 티어 크기로 맞춰 풀 오염을 막고,
	//   진짜 큰 custom은 페이지 단위로 올림해 풀에 넣지 않는다.
	static bool ResolveTier(CoStackTier _tier, _u32 _size, OUT CoStackTier* _pTier, OUT _u32* _pSize);
	static bool IsShadowStackEnabled();

	_u32		EmergencyPadPages(_u32 _totalPages) const; // 비상 패드 페이지 수. 전체의 1/4을 넘지 않게 한다.
	void		RecycleStack(CoStack* _pStack); // 풀 반납 시 커밋 유지 + 가드존 재배치. (커널 전이 최소화)

	// 테스트 전용: 내부 파라미터를 외부에서 설정한다.
	void		SetPageInitCount(_u32 _count) { pageInitCount_ = _count; }
	void		SetPageGuardCount(_u32 _count) { pageGuardCount_ = _count; }
	void		SetPageGrowCount(_u32 _count) { pageGrowCount_ = _count; }
	void		SetPageEmergencyCount(_u32 _count) { pageEmergencyCount_ = _count; } // 비상 페이지 수. 작은 스택에 다 안 들어가면 InitStack이 클램프한다.
	void		SetPoolKeepBytes(_u32 _bytes) { poolKeepBytes_ = _bytes; } // 풀 반납 스택이 유지할 커밋 상한. (이 아래는 디커밋)

private:
	bool		InitStack(CoStack* _pStack);
	bool		AllocStack(OUT CoStack* _pStack, CoStackTier _stackTier, _u32 _stackSize);
	void		FreeStack(CoStack* _pStack);

	_u32 pageInitCount_  = 2;
	_u32 pageGuardCount_ = 3;
	_u32 pageGrowCount_  = 2;	// 확장 시 한 번에 늘리는 페이지 수 (soft 오버플로우 방지 위해 1 이상 권장)
	// 비상 페이지 수. 오버플로우 때 SEH 디스패치가 돌 공간이다.
	_u32 pageEmergencyCount_ = CO_PAGE_EMERGENCY_COUNT;
	// 풀에 반납된 스택이 유지할 커밋 상한. (그 아래는 디커밋)
	_u32 poolKeepBytes_ = CO_STACK_SIZE_MID;
	// 티어별 풀 상한. 넘으면 반납 때 완전 해제한다. (티어 인덱스 직접 사용)
	_u32 poolMax_[cstReservedTierCount + 1] = { 0, 256, 64, 16 };
	_u64 nextId_         = 0;	// 단조 증가. Clear를 포함한 어떤 경로에서도 리셋 금지

	jc::LinkedList<CoContext*>	  free_[cstReservedTierCount + 1];
	jc::TreeMap<char*, CoContext*> usingByBase_;	// key: CoStack::pStackBase_ (주소 범위 조회용)
	jc::HashMap<_u64, CoContext*> usingById_;		// key: CoContext::id_ (검증용. 역참조 없음)
	// 티어별 슬랩. 64KB 그래뉴러리티 낭비 없이 1MB 예약을 분할한다.
	// - 각 슬롯은 아래(낮은 주소)부터 패드 + 스택. 이웃 경계는 각 슬롯 바닥의
	//   오버플로우 가드가 맡아서 안전성은 그대로다. 반납 슬롯 재사용은 free_ 풀 몫.
	struct StackSlab
	{
		char*	pBase_ = nullptr;	// 슬랩 예약 시작
		_u32	slotBytes_ = 0;		// 슬롯 1개 크기 (스택 + 아래 패드)
		_u32	slotCount_ = 0;		// 슬롯 개수
		_u32	used_ = 0;			// 할당한 슬롯 수 (bump pointer)
	};
	jc::LinkedList<StackSlab> slabs_[cstReservedTierCount + 1];	// 티어 인덱스 직접 사용
	static constexpr _u32 SLAB_BYTES = 1 << 20;	// 1MB
};

extern thread_local CoMgr g_cCoMgr;


extern "C"
{
	CoContext*	CPP_CALL CoAllocCtx(FnCoroutine _fn, CoStackTier _stackTier, _u32 _stackSize);
	void		CPP_CALL CoFreeCtx(CoContext* _ctx);
	CoContext*  CPP_CALL CoCurrentCtx();
	bool		CPP_CALL CoValidateAddr(CoContext* _pCtx, char* _pAddr);

	// 유일한 asm 진입점. 현재 레지스터를 save에 저장하고
	// load에서 복원한 뒤 ret로 복귀한다. (분기 없음, 호출 없음)
	void		ASM_CALL CoSwitchImpl(CoRegs* _pSave, const CoRegs* _pLoad);
	// 첫 진입 thunk. regs_.rbp_ = ctx 규약으로 CoEntry를 호출한다.
	void		ASM_CALL CoEntryThunk();

	CoError		CPP_CALL CoGetLastError();

	// 코루틴 진입점. fn을 noexcept 경계 안에서 호출하고,
	// 빠져나온 예외는 보관해 둔다. (asm이 fn_ 대신 이 함수로 점프한다)
	void		CPP_CALL CoEntry(CoContext* _pCtx) noexcept;
	// 보관된 예외가 있으면 스케줄러 스택에서 다시 던진다. (1회성)
	// - 던지는 동안 보관 소유권을 유지해야 해서 take 뒤에도 보관분이 남는다.
	//   catch 뒤에는 CoClearPendingException()으로 버릴 것. (잊어도 다음
	//   코루틴 시작 시 정리되지만, 그 전 take는 묵은 예외를 던지므로 주의)
	bool		CPP_CALL CoTakePendingException();
	// 보관된 예외를 버린다. (catch 뒤 호출)
	void		CPP_CALL CoClearPendingException();

	// 오버플로우를 잡은 뒤 계속 쓰려면 가드존을 다시 세운다.
	// - 현재 rsp 아래에 여유가 있어야 하며, 실패하면 false. (그럼 종료할 것)
	// - 성공해도 스택 위 객체들은 이미 망가졌을 수 있어 재개보다 종료를 권장한다.
	bool		CPP_CALL CoResetStackOverflow();
	// 커널이 올린 스택 오버플로우를 잡았다고 표시한다.
	// - 커널이 직접 올린 오버플로우는 우리를 거치지 않으므로 __except 안에서
	//   호출해 overflowed_를 찍는다.
	void		CPP_CALL CoNoteStackOverflow();
}

jc::String	CPP_CALL CoErrorString(CoError _err);

inline void CoYield()
{
	CoMgr& mgr = g_cCoMgr;
	CoContext* pCtx = mgr.currentCtx_;
	jc_assert_msg(pCtx != nullptr, _T("CoYield: 코루틴 밖에서 호출됨"));
	if (pCtx == nullptr)
		return;
#ifdef _DEBUG
	jc_assert(CoValidateAddr(pCtx, (char*)_AddressOfReturnAddress()));
#endif
	// 살아있는 한계를 stack_에 동기화한다. (커널 확장분 반영)
	pCtx->stack_.pStackLimit_ = (char*)__readgsqword(0x10);
	pCtx->state_ = csYield;
	mgr.currentCtx_ = pCtx->pCallerCtx_;
	CoSwitchImpl(&pCtx->regs_, &pCtx->callerRegs_);
	// 복귀 = resume됨. currentCtx_는 resume 쪽에서 이미 설정.
#ifdef _DEBUG
	CoMgr::VerifyGuardZone(pCtx->stack_);
#endif
}

// 스택 지정. 팩토리로만 만든다. 무효 조합은 문법적으로 성립하지 않는다.
struct CoStackSpec
{
	static CoStackSpec Mid()              { return {}; }
	static CoStackSpec Low()              { CoStackSpec s; s.tier_ = cstLow;  return s; }
	static CoStackSpec High()             { CoStackSpec s; s.tier_ = cstHigh; return s; }
	static CoStackSpec Custom(_u32 _size) { CoStackSpec s; s.tier_ = cstCustom; s.size_ = _size; return s; }
	static CoStackSpec FromTier(CoStackTier _tier) { CoStackSpec s; s.tier_ = _tier; return s; }

	CoStackTier tier_ = cstMid;
	_u32 size_ = 0; // cstCustom일 때만 사용
};

// 코루틴 생성 서술자.
struct CoDesc
{
	CoStackSpec spec_;
	// 시작 콘텍스트. fn 첫 줄부터 userData_로 보인다.
	void* userData_ = nullptr;
	// 생성된 컨텍스트를 받아둘 곳. 관찰·직접채널용이며 재개에 사용 금지.
	CoContext** ppOut_ = nullptr;
	// 예외 발생 시 호출될 콜백. 없으면 전역 핸들러 → 보관 순으로 처리된다.
	FnCoException onException_ = nullptr;
};

// 코루틴을 시작하고 첫 yield까지 실행한다. 실패하면 CO_INVALID_ID를 돌려준다.
CoId		CPP_CALL CoRun(FnCoroutine _fn, const CoDesc& _desc = {});
// id로 재개한다. yield 후 살아있으면 true, 종료·실패하면 false를 돌려준다.
// - false의 원인은 CoGetLastError()로 확인한다. (coeNone이면 정상 종료)
bool		CPP_CALL CoResume(CoId _id);
// 값 채널 버전. _inSwitchData를 넣고 재개한 뒤, 코루틴이 낸 값을 _pOut에 꺼낸다.
bool		CPP_CALL CoResume(CoId _id, _u64 _inSwitchData, _u64* _pOut = nullptr);
// 스레드별 전역 예외 핸들러를 등록한다. (개별 onException 다음, 보관 전)
void		CPP_CALL CoSetExceptionHandler(FnCoException _fn);

// 람다/함수자 지원.
// - 람다 객체는 힙에 두고 fn이 끝나면 지운다. 시작 자체가 실패하면 여기서 지운다.
//   (끝까지 돈 경우와 구분하려고 시작 플래그를 쓴다. 에러 코드 판별은 겹칠 수 있음)
// - _desc.userData_는 Starter가 차지하므로 무시된다.
template <typename TFn>
CoId CoRunFn(TFn&& _fn, const CoDesc& _desc = {})
{
	using DecayFn = std::decay_t<TFn>;
	struct Starter
	{
		DecayFn* pFn = nullptr;
		bool started = false;
	};
	DecayFn* pFn = dbg_new DecayFn(std::forward<TFn>(_fn));
	Starter starter{ pFn, false };
	CoDesc desc = _desc;
	desc.userData_ = &starter;
	CoId id = CoRun([](CoContext* _c)
	{
		Starter* pS = (Starter*)_c->userData_;
		pS->started = true;
		DecayFn* pOwn = pS->pFn;
		(*pOwn)(_c);
		delete pOwn;
	}, desc);
	if (!starter.started)
		delete pFn;
	return id;
}

// 값 채널. 코루틴 → 스케줄러로 내보내고, 다음 resume 때 값을 받는다.
inline _u64 CoYield(_u64 _outSwitchData)
{
	CoContext* pCtx = g_cCoMgr.currentCtx_;
	jc_assert_msg(pCtx != nullptr, _T("CoYield: 코루틴 밖에서 호출됨"));
	if (pCtx == nullptr)
		return 0;
	pCtx->switchData_ = _outSwitchData;
	CoYield();
	return pCtx->switchData_;
}

// 협력적 취소 + 자동 정리 스코프 가드.
//
// 예시:
//   {
//       CoScope sc(CoRun(Worker));
//       while (CoResume(sc.Id())) {}
//   } // 소멸 시 살아있으면 정책에 따라 처리
//   {
//       CoScope sc(CoRun(Service), CoScopeRule::CancelAssert);
//       sc.Cancel(); // 명시 종료. 소멸 시 assert 없음
//   }
enum class CoScopeRule
{
	CancelQuiet,	// 살아있으면 조용히 취소·drain
	CancelAssert,	// 살아있으면 Debug assert 후 drain
};

class CoScope
{
	CoId id_ = CO_INVALID_ID;
	CoScopeRule rule_ = CoScopeRule::CancelAssert;
public:
	explicit CoScope(CoId _id, CoScopeRule _rule = CoScopeRule::CancelAssert)
		: id_(_id), rule_(_rule) {}
	CoScope(CoScope&& _o) noexcept : id_(_o.id_), rule_(_o.rule_) { _o.id_ = CO_INVALID_ID; }
	CoScope(const CoScope&) = delete;
	~CoScope();

	CoId Id() const { return id_; }
	bool Resume() { return CoResume(id_); }

	// yield 상태 코루틴을 끝까지 돌려 정리한다.
	// - 스택 위 C++ 객체는 정상 복귀 경로로 소멸한다. fn이 CoCancelRequested()를 보고
	//   직접 return하는 협력적 취소를 권장한다.
	void Cancel();
};

// 취소 요청이 들어왔는지 확인한다. (fn 안에서 호출)
inline bool CoCancelRequested()
{
	CoContext* pCtx = CoCurrentCtx();
	return pCtx != nullptr && pCtx->isCancelRequested_;
}

LONG CALLBACK CoVEH(EXCEPTION_POINTERS* _pEp) noexcept;
