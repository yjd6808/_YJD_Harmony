; ========================================================================
; 작성자 : 윤정도
; 코루틴 어셈블리 구현
; - CoRun, CoYield, CoResume 프로시저
; - CoFnEndTrampoline 트램폴린
; - Windows x64 callee-saved 레지스터 전부 저장/복원
; ========================================================================

option casemap: NONE
include ..\Sources\jc\_Extern\Extern.asm

extern CoAllocCtx       : proc
extern CoFreeCtx        : proc
extern CoCurrentCtx     : proc
extern CoValidateAddr   : proc
extern CoValidateResume : proc
extern CoOnBeforeLaunch : proc
extern CoOnAfterLaunch  : proc
extern CoEntry          : proc      ; [코루틴-01] 코루틴 진입점 (fn 대신 점프)

; TEB 오프셋 (참고용. DeallocationStack은 교체하지 않음. [코루틴-02])
TEB_DEALLOCATION_STACK EQU 1478h

; ============================================================
;  CoStackTier enum
; ============================================================
cstNone              EQU 0
cstLow               EQU 1
cstMid               EQU 2
cstHigh              EQU 3
cstCustom            EQU 4

; ============================================================
;  CoState enum
; ============================================================
csInit               EQU 0
csRun                EQU 1
csYield              EQU 2
csEnd                EQU 3

; ============================================================
CoStack struct 8
    size_        DWORD   ?
    _pad0_       DWORD   ?
    pStackBase_  QWORD   ?
    pStackEnd_   QWORD   ?
    pStackLimit_ QWORD   ?
    pGuardLimit_ QWORD   ?
    stackTier_   DWORD   ?
    _pad1_       DWORD   ?
    magic_       QWORD   ?           ; [코루틴-05] 스택 식별 매직 (C++ CoStack과 순서/크기 일치)
    pEmergencyTop_ QWORD ?           ; [코루틴-04] 비상 밴드 상단 (C++와 일치)
    pReserveBase_ QWORD  ?           ; [코루틴-04] 예약 시작 주소 (C++와 일치)
    overflowed_  BYTE    ?           ; [코루틴-04] 오버플로우 처리 여부 (C++와 일치)
    _pad2_       BYTE    7 dup(?)    ; sizeof(CoStack) = 80
CoStack ends

OFFSET_COSTACK_SIZE        EQU CoStack.size_
OFFSET_COSTACK_STACKBASE   EQU CoStack.pStackBase_
OFFSET_COSTACK_BASEEND     EQU CoStack.pStackBase_     ; EndAddr는 StackEnd와 동일하게 사용
OFFSET_COSTACK_STACKEND    EQU CoStack.pStackEnd_
OFFSET_COSTACK_BASEADDR    EQU CoStack.pStackEnd_
OFFSET_COSTACK_STACKLIMIT  EQU CoStack.pStackLimit_
OFFSET_COSTACK_GUARDLIMIT  EQU CoStack.pGuardLimit_
OFFSET_COSTACK_STACKTIER   EQU CoStack.stackTier_
OFFSET_COSTACK_MAGIC       EQU CoStack.magic_
OFFSET_COSTACK_EMERGENCYTOP EQU CoStack.pEmergencyTop_
OFFSET_COSTACK_RESERVEBASE EQU CoStack.pReserveBase_
OFFSET_COSTACK_OVERFLOWED  EQU CoStack.overflowed_

; ============================================================
CoRegs struct 8
    rip_    QWORD   ?           ; offset   0
    rsp_    QWORD   ?           ; offset   8
    rbp_    QWORD   ?           ; offset  16

    gs8_    QWORD   ?           ; offset  24  TEB StackBase:  0x8
    gs16_   QWORD   ?           ; offset  32  TEB StackLimit: 0x10
    gs1478_ QWORD   ?           ; offset  40  TEB DeallocationStack: 0x1478 [코루틴-02/04]

    ; Windows x64 callee-saved 정수 레지스터
    rsi_    QWORD   ?           ; offset  48
    rdi_    QWORD   ?           ; offset  56
    r12_    QWORD   ?           ; offset  64
    r13_    QWORD   ?           ; offset  72
    r14_    QWORD   ?           ; offset  80
    r15_    QWORD   ?           ; offset  88

    ; [코루틴-08] 부동소수점 제어 상태 (C++ CoRegs와 순서/크기 일치)
    mxcsr_  DWORD   ?           ; offset  96
    fpucw_  WORD    ?           ; offset 100
    _padFp_ WORD    ?           ; offset 102

    ; Windows x64 callee-saved XMM 레지스터 (16 bytes each, 8-byte aligned)
    xmm6_   BYTE    16 dup(?)   ; offset 104
    xmm7_   BYTE    16 dup(?)   ; offset 120
    xmm8_   BYTE    16 dup(?)   ; offset 136
    xmm9_   BYTE    16 dup(?)   ; offset 152
    xmm10_  BYTE    16 dup(?)   ; offset 168
    xmm11_  BYTE    16 dup(?)   ; offset 184
    xmm12_  BYTE    16 dup(?)   ; offset 200
    xmm13_  BYTE    16 dup(?)   ; offset 216
    xmm14_  BYTE    16 dup(?)   ; offset 232
    xmm15_  BYTE    16 dup(?)   ; offset 248
    ; sizeof(CoRegs) = 264 (C++와 EQU로 일치. 어긋나면 09의 static_assert가 잡음)

CoRegs ends

OFFSET_COREGS_RIP   EQU CoRegs.rip_
OFFSET_COREGS_RSP   EQU CoRegs.rsp_
OFFSET_COREGS_RBP   EQU CoRegs.rbp_
OFFSET_COREGS_GS8   EQU CoRegs.gs8_
OFFSET_COREGS_GS16  EQU CoRegs.gs16_
OFFSET_COREGS_GS1478 EQU CoRegs.gs1478_
OFFSET_COREGS_RSI   EQU CoRegs.rsi_
OFFSET_COREGS_RDI   EQU CoRegs.rdi_
OFFSET_COREGS_R12   EQU CoRegs.r12_
OFFSET_COREGS_R13   EQU CoRegs.r13_
OFFSET_COREGS_R14   EQU CoRegs.r14_
OFFSET_COREGS_R15   EQU CoRegs.r15_
OFFSET_COREGS_MXCSR EQU CoRegs.mxcsr_
OFFSET_COREGS_FPUCW EQU CoRegs.fpucw_
OFFSET_COREGS_XMM6  EQU CoRegs.xmm6_
OFFSET_COREGS_XMM7  EQU CoRegs.xmm7_
OFFSET_COREGS_XMM8  EQU CoRegs.xmm8_
OFFSET_COREGS_XMM9  EQU CoRegs.xmm9_
OFFSET_COREGS_XMM10 EQU CoRegs.xmm10_
OFFSET_COREGS_XMM11 EQU CoRegs.xmm11_
OFFSET_COREGS_XMM12 EQU CoRegs.xmm12_
OFFSET_COREGS_XMM13 EQU CoRegs.xmm13_
OFFSET_COREGS_XMM14 EQU CoRegs.xmm14_
OFFSET_COREGS_XMM15 EQU CoRegs.xmm15_

; ============================================================
CoContext struct 8
    id_         DWORD   ?
    threadId_   DWORD   ?
    generation_ DWORD   ?           ; [코루틴-05] 세대 번호 (C++ CoContext와 순서/크기 일치)
    _padGen_    DWORD   ?
    regs_       CoRegs  <>
    stack_      CoStack <>
    state_      DWORD   ?
    _pad0_      DWORD   ?
    fn_         QWORD   ?
    callerCtx_  QWORD   ?
CoContext ends

OFFSET_COCTX_ID       EQU CoContext.id_
OFFSET_COCTX_THREADID EQU CoContext.threadId_
OFFSET_COCTX_GEN      EQU CoContext.generation_
OFFSET_COCTX_REGS     EQU CoContext.regs_
OFFSET_COCTX_STACK    EQU CoContext.stack_
OFFSET_COCTX_STATE    EQU CoContext.state_
OFFSET_COCTX_FN       EQU CoContext.fn_
OFFSET_COCTX_CALLER   EQU CoContext.callerCtx_

code

; ============================================================
; CoFnEndTrampoline
;   CoEntry()가 ret할 때 진입하는 트램폴린 (코루틴 스택 활성화 상태)
;
; [코루틴-01] 진입 시 스택 레이아웃 (pStackBase_ 기준):
;   [pStackBase_ -  8] = 0 (가짜 반환주소. 언와인더가 여기서 멈춤)
;   [pStackBase_ - 16] = CoContext* (CoRun이 백업)
;   [pStackBase_ - 48] ← RSP (0 mod 16, 트램폴린 프레임 40)
; 언와인드 경로: fn → CoEntry → Resume → .allocstack 40 해제 → base-8 pop = 0 → 종료.
; 디버거 콜스택은 fn ← CoFnEndTrampoline 에서 깔끔하게 끝난다.
;
; 역할:
;   1. CoCurrentCtx로 현재 CoContext 탐색 ([rsp+32] 백업 덕에 shadow 추가 없이 call)
;   2. 스레드 TEB 3종 / 정수 / XMM / 부동소수점 제어 상태 복원
;   3. state = csEnd 설정
;   4. 스케줄러 스택으로 전환 후 YIELD 레이블로 점프
; ============================================================
CoFnEndTrampoline proc FRAME
    sub     rsp,    40
    .allocstack 40
    .endprolog
CoFnEndTrampoline_Resume::
    ; 진입: RSP = pStackBase_-48 (0 mod 16). 프레임 40 안에 shadow가 있으므로 바로 call.
    ; (call이 미는 반환주소는 base-56에 얹히고, [rsp+32] 백업은 shadow 밖에 있어 안전)
    call    CoCurrentCtx

    ; rax = CoContext* (null이면 프로그래밍 오류 - unreachable)
    cmp     rax,    0
    jnz     CTX_OK
    int     3                           ; 코루틴 스택에서 컨텍스트를 찾지 못함
CTX_OK:

    ; 스레드 TEB 복원 (StackBase/Limit + DeallocationStack)
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    mov     gs:[8],  r10
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    mov     gs:[16], r10
    ; [코루틴-02/04] DeallocationStack 복원 (비상 밴드 상단으로 설치했던 값)
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478]
    mov     gs:[TEB_DEALLOCATION_STACK], r10

    ; [코루틴-08] 스레드 부동소수점 제어 상태 복원
    ldmxcsr [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR]
    fldcw   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW]

    ; 컨텍스트의 callee-saved 정수 레지스터 복원
    mov     rsi,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI]
    mov     rdi,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI]
    mov     r12,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12]
    mov     r13,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13]
    mov     r14,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14]
    mov     r15,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15]

    ; 컨텍스트의 callee-saved XMM 레지스터 복원 (movdqu: alignment 미보장)
    movdqu  xmm6,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6]
    movdqu  xmm7,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7]
    movdqu  xmm8,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8]
    movdqu  xmm9,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9]
    movdqu  xmm10,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10]
    movdqu  xmm11,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11]
    movdqu  xmm12,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12]
    movdqu  xmm13,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13]
    movdqu  xmm14,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14]
    movdqu  xmm15,  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15]

    ; state = csEnd
    mov     dword ptr[rax + OFFSET_COCTX_STATE], csEnd

    ; 컨텍스트 스택으로 전환 후 YIELD 레이블로 점프
    ; regs_.rsp_ = 컨텍스트 스택 (CoContext* push 직후, top = CoContext*)
    ; regs_.rip_ = 컨텍스트의 YIELD 레이블 (CoRun 또는 CoResume)
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP]
    mov     r11,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP]
    mov     rbp,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP]
    mov     rsp,    r10
    jmp     r11

CoFnEndTrampoline endp

; ============================================================
CoRun proc FRAME
    push    rbp
    .pushreg rbp
    push    rbx
    .pushreg rbx
    mov	    rbp,    rsp
    .setframe rbp, 0
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte
    .allocstack 40
    .endprolog

    ; 전달받은 인자 그대로 이어서 전달
    call    CoAllocCtx
    cmp     rax,    0
    jne     ALLOC_OK
    ; [코루틴-07] 할당 실패(null fn, 예약/커밋 실패)는 nullptr을 돌려준다.
    ; - 이전에는 FIN이 mov rax,rcx로 복귀했는데 rcx는 call에 의해 깨진 값이므로
    ;   쓰레기 포인터가 반환됐다. 실패 원인은 CoGetLastError()로 확인한다.
    xor     ecx,    ecx
    jmp     FIN
ALLOC_OK:

    push	rax                     ; CoContext 포인터 백업 (컨텍스트 스택)

    lea     rbx,    YIELD           ; CoYield / 트램폴린에서 복귀할 때 사용할 주소
    ; 스레드 TEB 저장 (r10/r11만 사용)
    mov     r10,    gs:[8]          ; 기존 StackBase 저장
    mov     r11,    gs:[16]         ; 기존 StackLimit 저장
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],     r10
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16],    r11
    mov     r10,    gs:[TEB_DEALLOCATION_STACK] ; 기존 DeallocationStack 저장
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478],  r10
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP],     rsp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP],     rbx
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP],     rbp

    ; 컨텍스트의 callee-saved 정수 레지스터를 regs_에 저장
    ; (코루틴 최초 실행 전 초기화: CoYield xchg 시 올바른 컨텍스트 값 복원을 위해)
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI],     rsi
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI],     rdi
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12],     r12
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13],     r13
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14],     r14
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15],     r15

    ; 컨텍스트의 callee-saved XMM 레지스터를 regs_에 저장 (movdqu: alignment 미보장)
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6],    xmm6
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7],    xmm7
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8],    xmm8
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9],    xmm9
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10],   xmm10
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11],   xmm11
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12],   xmm12
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13],   xmm13
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14],   xmm14
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15],   xmm15

    ; [코루틴-08] 스레드 부동소수점 제어 상태 저장 (코루틴은 이 값으로 시작)
    stmxcsr [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR]
    fnstcw  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW]

    mov     dword ptr[rax + OFFSET_COCTX_STATE],   csRun

    mov     r10,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKBASE]
    mov     r11,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKLIMIT]
    mov     gs:[8],     r10         ; StackBase  = 코루틴 스택 Base (r10은 아래 rsp 전환에 재사용)
    mov     gs:[16],    r11         ; StackLimit = 코루틴 스택 Limit (초기 커밋)
    ; [코루틴-02/04] DeallocationStack = 비상 밴드 상단.
    ; - 예약 하단을 설치하면 커널이 가드 폴트를 직접 확장해 VEH가 안 불리므로(실측),
    ;   밴드 상단을 경계로 둔다. 그 아래는 커널이 오버플로우로 확정한다.
    mov     r11,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_EMERGENCYTOP]
    mov     gs:[TEB_DEALLOCATION_STACK], r11

    ; --- 코루틴 스택 진입 ([코루틴-01] 가짜 프레임 + 반환주소 0) ---
    ; call 대신 push-trampoline + jmp 패턴은 유지하되, 언와인더가 멈출 수 있게
    ; 트램폴린 아래에 가짜 프레임(40, .allocstack과 일치)과 반환주소 0을 깐다.
    mov     rsp,        r10             ; RSP = pStackBase_ (0 mod 16)
    push    0                           ; 가짜 반환주소 = 0 → 언와인더 종료 (8 mod 16)
    sub     rsp,        40              ; 트램폴린 프레임 (0 mod 16)
    mov     [rsp+32],   rax             ; 트램폴린이 꺼내 쓸 ctx 백업

    ; CoOnBeforeLaunch(CoContext*) — CoEntry 실행 직전
    ;   call이 미는 반환주소는 base-56에 얹히고, [rsp+32] 백업은 shadow 밖에 있어 안전.
    mov     rcx,        rax
    call    CoOnBeforeLaunch            ; 진입 rsp = base-56 (8 mod 16) ✓
    mov     rax,        [rsp+32]        ; rax = CoContext* 복원, RSP = pStackBase_-48

    ; [코루틴-01] fn_ 대신 CoEntry로 점프. fn의 예외는 CoEntry가 잡아 둔다.
    lea     r10,        CoEntry
    lea     r11,        CoFnEndTrampoline_Resume
    push    r11                         ; CoEntry의 반환주소 = 트램폴린 본문 (8 mod 16)
    mov     rcx,        rax             ; arg: CoContext*
    jmp     r10                         ; CoEntry(CoContext*)

YIELD:
    ; 컨텍스트 스택 활성화 상태
    ; (CoYield 또는 CoFnEndTrampoline이 컨텍스트 스택 전환 후 점프)
    ; 컨텍스트 스택 top: push rax로 저장해놓은 CoContext 포인터
    ; RSP = 8 mod 16, [RSP] = CoContext*

    ; CoOnAfterLaunch(CoContext*) — 컨텍스트 복귀 직후
    ;   mov rcx [rsp] (peek, pop 없이)
    ;   sub rsp, 40 (shadow 32 + align 8) → RSP = 8-40 = 0 mod 16 ✓
    mov     rcx,        [rsp]
    sub     rsp,        40
    call    CoOnAfterLaunch
    add     rsp,        40              ; RSP = 8 mod 16 복원

    pop     rcx
    mov     ebx,        [rcx + OFFSET_COCTX_STATE]
    cmp     ebx,        csYield
    jz      FIN

    ; csEnd: fn()이 정상 종료됨 → 컨텍스트 해제 후 nullptr 반환
    call    CoFreeCtx
    mov     rcx,        0
FIN:
    mov     rax,        rcx
    mov     rsp,        rbp
    pop     rbx
    pop     rbp
    ret
CoRun endp


; [코루틴-07] 기존 CoYield를 C++ 인라인 래퍼 뒤로 숨긴다.
; - 코루틴 밖에서 호출하면 null 컨텍스트를 역참조해 크래시나므로,
;   헤더의 inline CoYield()가 먼저 검사하고 여기서부터는 항상 유효한 호출이다.
CoYieldImpl proc FRAME
    push    rbp
    .pushreg rbp
    push    rbx
    .pushreg rbx
    mov     rbp,    rsp
    .setframe rbp, 0
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte
    .allocstack 40
    .endprolog

    ; 현재 실행 중인 코루틴 컨텍스트 취득 (O(1) thread_local)
    call    CoCurrentCtx            ; 인자 없음, shadow space 기할당
    cmp     rax,    0
    jz      FIN
    mov     rbx,    rax             ; rbx = CoContext* 임시 보관

    ; RSP(=rbp)가 해당 컨텍스트의 스택 범위 내인지 검증
    mov     rcx,    rbx             ; arg1: CoContext*
    mov     rdx,    rbp             ; arg2: 코루틴 스택 내 주소
    call    CoValidateAddr          ; rax = bool
    cmp     rax,    0
    jz      FIN
    mov     rax,    rbx             ; rax = CoContext* 복원

    ; 컨텍스트 스위치 수행

    ; StackBase mov 교환 (r10/r11만 사용. LOCK이 걸리는 xchg mem 대신)
    mov     r10,    gs:[8]                                      ; 현재(코루틴)
    mov     r11,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8] ; 저장(스레드)
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],  r10
    mov     gs:[8], r11

    ; StackLimit: 현재(코루틴) 한계를 stack_에 동기화 후 스레드 값 복원 ([코루틴-03])
    ; - 커널이 가드 폴트를 직접 확장할 수 있어 pStackLimit_가 뒤처질 수 있으므로,
    ;   yield 시점에 살아있는 TEB 값을 기준으로 맞춘다. resume은 이 값을 읽는다.
    mov     r10,    gs:[16]
    mov     [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKLIMIT], r10
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    mov     gs:[16], r10

    ; DeallocationStack mov 교환 ([코루틴-02/04] 비상 밴드 상단 ↔ 스레드 값)
    mov     r10,    gs:[TEB_DEALLOCATION_STACK]
    mov     r11,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478], r10
    mov     gs:[TEB_DEALLOCATION_STACK], r11

    lea     rbx,    FIN
    xchg    rbx,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP]

    mov     r10,    rsp
    mov     r11,    rbp

    mov     rsp,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP]
    mov     rbp,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP]

    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP],    r10
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP],    r11

    ; callee-saved 정수 레지스터 xchg
    ; 패턴: r10 = regs_[reg], regs_[reg] = reg, reg = r10
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI],    rsi
    mov     rsi,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI],    rdi
    mov     rdi,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12],    r12
    mov     r12,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13],    r13
    mov     r13,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14],    r14
    mov     r14,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15],    r15
    mov     r15,    r10

    ; callee-saved XMM 레지스터 xchg (xmm0은 volatile 임시 레지스터로 사용)
    ; 패턴: xmm0 = regs_[xmmN], regs_[xmmN] = xmmN (movdqu), xmmN = xmm0 (movaps reg←reg)
    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6],    xmm6
    movaps  xmm6,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7],    xmm7
    movaps  xmm7,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8],    xmm8
    movaps  xmm8,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9],    xmm9
    movaps  xmm9,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10],   xmm10
    movaps  xmm10,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11],   xmm11
    movaps  xmm11,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12],   xmm12
    movaps  xmm12,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13],   xmm13
    movaps  xmm13,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14],   xmm14
    movaps  xmm14,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15],   xmm15
    movaps  xmm15,  xmm0

    ; [코루틴-08] MXCSR/x87 교환 (현재 부동소수점 상태를 저장하고 상대 값을 로드)
    ; - x87 예외는 기본 마스크되어 있어 fldcw 전 fnclex가 필요 없다.
    sub     rsp, 8
    stmxcsr [rsp]
    fnstcw  [rsp+4]
    ldmxcsr [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR]
    fldcw   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW]
    mov     r10d, [rsp]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR], r10d
    mov     r10w, [rsp+4]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW], r10w
    add     rsp, 8

    mov     dword ptr[rax + OFFSET_COCTX_STATE],              csYield
    jmp     rbx

FIN:
    mov     rsp,    rbp
    pop     rbx
    pop     rbp
    ret
CoYieldImpl endp

CoResume proc FRAME
    push    rbp
    .pushreg rbp
    push    rbx
    .pushreg rbx
    mov     rbp,    rsp
    .setframe rbp, 0
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte
    .allocstack 40
    .endprolog

    cmp     rcx,    0
    jz      FIN

    ; [코루틴-05] resume 전 검사 (스레드/상태/자기 자신).
    ; - 이전에는 null 검사만 해서 다른 스레드나 종료된 컨텍스트를 resume하면
    ;   정의되지 않은 곳으로 점프했다. 실패 시 nullptr과 에러 코드를 돌려준다.
    ; - rsp는 0 mod 16이므로 shadow space가 이미 확보되어 바로 call한다.
    mov     rbx,    rcx                 ; ctx 보관 (rbx는 callee-saved, push로 보존됨)
    call    CoValidateResume            ; rcx = ctx
    test    al,     al
    jz      FIN_NULL
    mov     rcx,    rbx

    push    rcx                     ; CoContext 포인터 백업 (컨텍스트 스택)
    mov     rax,    rcx

    ; StackBase mov 교환 (스레드 값 저장 + 코루틴 값 설치)
    mov     r10,        gs:[8]
    mov     r11,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],     r10
    mov	    gs:[8],     r11

    ; StackLimit 스레드 값 저장 + 코루틴 현재값 로드
    ; ([코루틴-03] yield 시점에 동기화된 한계를 읽는다. VEH 확장이 있으면 그 값)
    mov     r11,        gs:[16]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16],    r11
    mov     r11,        [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKLIMIT]
    mov     gs:[16],    r11

    ; DeallocationStack mov 교환 ([코루틴-02/04] 비상 밴드 상단 ↔ 스레드 값)
    mov     r10,        gs:[TEB_DEALLOCATION_STACK]
    mov     r11,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS1478],  r10
    mov     gs:[TEB_DEALLOCATION_STACK], r11

    ; rbp xchg
    mov     rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP]
    xchg    rbx,        rbp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP],     rbx

    ; callee-saved 정수 레지스터 xchg (r10을 임시 레지스터로 사용)
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSI],    rsi
    mov     rsi,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RDI],    rdi
    mov     rdi,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R12],    r12
    mov     r12,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R13],    r13
    mov     r13,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R14],    r14
    mov     r14,    r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_R15],    r15
    mov     r15,    r10

    ; callee-saved XMM 레지스터 xchg (xmm0은 volatile 임시 레지스터로 사용)
    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM6],    xmm6
    movaps  xmm6,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM7],    xmm7
    movaps  xmm7,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM8],    xmm8
    movaps  xmm8,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM9],    xmm9
    movaps  xmm9,   xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM10],   xmm10
    movaps  xmm10,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM11],   xmm11
    movaps  xmm11,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM12],   xmm12
    movaps  xmm12,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM13],   xmm13
    movaps  xmm13,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM14],   xmm14
    movaps  xmm14,  xmm0

    movdqu  xmm0,   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15]
    movdqu  [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_XMM15],   xmm15
    movaps  xmm15,  xmm0

    ; [코루틴-08] MXCSR/x87 교환 (CoYieldImpl과 동일. rsp 전환 전에 임시 8바이트 사용)
    sub     rsp, 8
    stmxcsr [rsp]
    fnstcw  [rsp+4]
    ldmxcsr [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR]
    fldcw   [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW]
    mov     r10d, [rsp]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_MXCSR], r10d
    mov     r10w, [rsp+4]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_FPUCW], r10w
    add     rsp, 8

    ; rsp & rip 복구 (코루틴의 저장된 값)
    mov     r10,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP]
    mov     r11,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP]

    mov     dword ptr[rax + OFFSET_COCTX_STATE],   csRun
    lea     rbx,        YIELD           ; CoYield / 트램폴린에서 복귀할 때 사용할 주소
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP],      rsp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP],      rbx

    ; 커스텀 스택으로 전환 후 코루틴 재개
    mov     rsp,        r10             ; 코루틴 스택으로 전환 (RSP = 코루틴 저장 RSP, 0 mod 16)

    ; CoOnBeforeLaunch(CoContext*) — 코루틴 재개 직전
    ;   push rax  → RSP-8  (8 mod 16)
    ;   sub  rsp, 40 (shadow 32 + align 8) → RSP-48 (0 mod 16) ✓
    push    rax
    mov     rcx,        rax
    sub     rsp,        40
    call    CoOnBeforeLaunch
    add     rsp,        40
    pop     rax                         ; rax = CoContext* 복원, RSP 원위치

    jmp     r11

YIELD:
    ; 컨텍스트 스택 활성화 상태
    ; (CoYield 또는 CoFnEndTrampoline이 컨텍스트 스택 전환 후 점프)
    ; 컨텍스트 스택 top: push rcx로 저장해놓은 CoContext 포인터
    ; RSP = 8 mod 16, [RSP] = CoContext*

    ; CoOnAfterLaunch(CoContext*) — 컨텍스트 복귀 직후
    mov     rcx,        [rsp]
    sub     rsp,        40
    call    CoOnAfterLaunch
    add     rsp,        40              ; RSP = 8 mod 16 복원

    pop     rcx
    mov     ebx,        [rcx + OFFSET_COCTX_STATE]
    cmp     ebx,        csYield
    jz      FIN

    ; csEnd: fn()이 정상 종료됨 → 컨텍스트 해제 후 nullptr 반환
    call    CoFreeCtx
    mov     rcx,        0
FIN:
    mov     rax,        rcx
    mov     rsp,        rbp
    pop     rbx
    pop     rbp
    ret
FIN_NULL:
    ; [코루틴-05] 검증 실패 경로. rax = nullptr로 복귀한다.
    xor     eax,        eax
    mov     rsp,        rbp
    pop     rbx
    pop     rbp
    ret
CoResume endp


end