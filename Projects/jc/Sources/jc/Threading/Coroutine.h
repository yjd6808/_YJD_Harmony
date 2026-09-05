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
};

struct CoRegs
{
	_u64 rip_  = 0;		// offset  0
	_u64 rsp_  = 0;		// offset  8
	_u64 rbp_  = 0;		// offset 16

	// 스위칭전 TEB의 gs:[8]과 gs:[16]에 저장된 StackBase와 StackLimit을 보관하는 용도
	_u64 gs8_  = 0;		// offset 24  TEB StackBase:  0x8
	_u64 gs16_ = 0;		// offset 32  TEB StackLimit: 0x10

	// Windows x64 callee-saved 정수 레지스터
	_u64 rsi_  = 0;		// offset 40
	_u64 rdi_  = 0;		// offset 48
	_u64 r12_  = 0;		// offset 56
	_u64 r13_  = 0;		// offset 64
	_u64 r14_  = 0;		// offset 72
	_u64 r15_  = 0;		// offset 80

	// Windows x64 callee-saved XMM 레지스터 (16 bytes each, 8-byte aligned)
	_u8 xmm6_[16]  = {};	// offset  88
	_u8 xmm7_[16]  = {};	// offset 104
	_u8 xmm8_[16]  = {};	// offset 120
	_u8 xmm9_[16]  = {};	// offset 136
	_u8 xmm10_[16] = {};	// offset 152
	_u8 xmm11_[16] = {};	// offset 168
	_u8 xmm12_[16] = {};	// offset 184
	_u8 xmm13_[16] = {};	// offset 200
	_u8 xmm14_[16] = {};	// offset 216
	_u8 xmm15_[16] = {};	// offset 232
	// sizeof(CoRegs) = 248
};

using FnCoroutine = void(*)(CoContext*);

struct CoContext
{
	_u32		id_ = 0;
	_u32		threadId_ = 0;
	CoRegs		regs_;
	CoStack		stack_;
	CoState		state_;
	FnCoroutine	fn_ = nullptr;
	CoContext*	callerCtx_ = nullptr;	// CoOnBeforeLaunch 진입 시 이전 currentCtx_ 보관
};

#pragma pack(pop)

class CoMgr
{
public:
	// [코루틴-07] 첫 사용 시 CoVEH를 프로세스에 1회 자동 등록한다.
	// - 이전에는 사용자가 직접 AddVectoredExceptionHandler를 호출해야 해서
	//   등록을 빼먹으면 첫 스택 확장 시점에 가드 폴트가 처리되지 않고 종료됐다.
	CoMgr();

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
	bool		ExpandStack(CoStack* _pStack, char* _pFaultAddr);
	void		DumpStack(CoStack* _pStack, const char* _pTitle = nullptr);
	void		Clear();

	// ── 현재 실행 중인 코루틴 컨텍스트 (O(1) 접근) ───────────────────────────
	CoContext*	GetCurrentCtx() const { return currentCtx_; }
	CoContext*	currentCtx_ = nullptr;	// CoOnBeforeLaunch/AfterLaunch에서 직접 설정

	// 테스트 전용: 내부 파라미터를 외부에서 설정한다.
	void		SetPageInitCount(_u32 _count) { pageInitCount_ = _count; }
	void		SetPageGuardCount(_u32 _count) { pageGuardCount_ = _count; }
	void		SetPageGrowCount(_u32 _count) { pageGrowCount_ = _count; }

private:
	// [코루틴-07] 커밋 실패를 호출자에게 알리기 위해 bool을 돌려준다.
	bool		InitStack(CoStack* _pStack);
	bool		AllocStack(OUT CoStack* _pStack, CoStackTier _stackTier, _u32 _stackSize);
	void		FreeStack(CoStack* _pStack);

	_u32 pageInitCount_  = 2;
	_u32 pageGuardCount_ = 3;
	_u32 pageGrowCount_  = 2;	// 확장 시 한 번에 늘리는 페이지 수 (soft 오버플로우 방지 위해 1 이상 권장)
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

LONG CALLBACK CoVEH(EXCEPTION_POINTERS* _pEp);