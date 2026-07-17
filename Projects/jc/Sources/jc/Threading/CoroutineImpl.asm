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
extern CoOnBeforeLaunch : proc
extern CoOnAfterLaunch  : proc

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
CoStack ends

OFFSET_COSTACK_SIZE        EQU CoStack.size_
OFFSET_COSTACK_STACKBASE   EQU CoStack.pStackBase_
OFFSET_COSTACK_BASEEND     EQU CoStack.pStackBase_     ; EndAddr는 StackEnd와 동일하게 사용
OFFSET_COSTACK_STACKEND    EQU CoStack.pStackEnd_
OFFSET_COSTACK_BASEADDR    EQU CoStack.pStackEnd_
OFFSET_COSTACK_STACKLIMIT  EQU CoStack.pStackLimit_
OFFSET_COSTACK_GUARDLIMIT  EQU CoStack.pGuardLimit_
OFFSET_COSTACK_STACKTIER   EQU CoStack.stackTier_

; ============================================================
CoRegs struct 8
    rip_    QWORD   ?           ; offset   0
    rsp_    QWORD   ?           ; offset   8
    rbp_    QWORD   ?           ; offset  16

    gs8_    QWORD   ?           ; offset  24  TEB StackBase:  0x8
    gs16_   QWORD   ?           ; offset  32  TEB StackLimit: 0x10

    ; Windows x64 callee-saved 정수 레지스터
    rsi_    QWORD   ?           ; offset  40
    rdi_    QWORD   ?           ; offset  48
    r12_    QWORD   ?           ; offset  56
    r13_    QWORD   ?           ; offset  64
    r14_    QWORD   ?           ; offset  72
    r15_    QWORD   ?           ; offset  80

    ; Windows x64 callee-saved XMM 레지스터 (16 bytes each, 8-byte aligned)
    xmm6_   BYTE    16 dup(?)   ; offset  88
    xmm7_   BYTE    16 dup(?)   ; offset 104
    xmm8_   BYTE    16 dup(?)   ; offset 120
    xmm9_   BYTE    16 dup(?)   ; offset 136
    xmm10_  BYTE    16 dup(?)   ; offset 152
    xmm11_  BYTE    16 dup(?)   ; offset 168
    xmm12_  BYTE    16 dup(?)   ; offset 184
    xmm13_  BYTE    16 dup(?)   ; offset 200
    xmm14_  BYTE    16 dup(?)   ; offset 216
    xmm15_  BYTE    16 dup(?)   ; offset 232
    ; sizeof(CoRegs) = 248

CoRegs ends

OFFSET_COREGS_RIP   EQU CoRegs.rip_
OFFSET_COREGS_RSP   EQU CoRegs.rsp_
OFFSET_COREGS_RBP   EQU CoRegs.rbp_
OFFSET_COREGS_GS8   EQU CoRegs.gs8_
OFFSET_COREGS_GS16  EQU CoRegs.gs16_
OFFSET_COREGS_RSI   EQU CoRegs.rsi_
OFFSET_COREGS_RDI   EQU CoRegs.rdi_
OFFSET_COREGS_R12   EQU CoRegs.r12_
OFFSET_COREGS_R13   EQU CoRegs.r13_
OFFSET_COREGS_R14   EQU CoRegs.r14_
OFFSET_COREGS_R15   EQU CoRegs.r15_
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
    regs_       CoRegs  <>
    stack_      CoStack <>
    state_      DWORD   ?
    _pad0_      DWORD   ?
    fn_         QWORD   ?
    callerCtx_  QWORD   ?
CoContext ends

OFFSET_COCTX_ID       EQU CoContext.id_
OFFSET_COCTX_THREADID EQU CoContext.threadId_
OFFSET_COCTX_REGS     EQU CoContext.regs_
OFFSET_COCTX_STACK    EQU CoContext.stack_
OFFSET_COCTX_STATE    EQU CoContext.state_
OFFSET_COCTX_FN       EQU CoContext.fn_
OFFSET_COCTX_CALLER   EQU CoContext.callerCtx_

code

; ============================================================
; CoFnEndTrampoline
;   fn()이 정상 종료(ret)할 때 진입하는 트램폴린
;   CoRun이 call 대신 push-trampoline + jmp 패턴을 사용하므로
;   fn()의 ret이 여기로 도착함 (코루틴 스택 활성화 상태)
;
; 역할:
;   1. CoCurrentCtx로 현재 CoContext 탐색
;   2. 컨텍스트의 GS / RSI~R15 / XMM6~15 복원
;   3. state = csEnd 설정
;   4. 컨텍스트 스택으로 전환 + 컨텍스트의 RBP 복원
;   5. 컨텍스트의 YIELD 레이블로 점프
;      → YIELD: pop rcx(CoContext*), csEnd 감지, CoFreeCtx 호출, 0 반환
;
; 진입 시 스택 레이아웃 (pStackBase_ 기준):
;   [pStackBase_ - 32] ← RSP (16-byte 정렬)
;   [pStackBase_ - 40]   = CoFnEndTrampoline 주소 (fn ret이 이미 pop 완료)
;   shadow space: [pStackBase_-32 .. pStackBase_-9]
; ============================================================
CoFnEndTrampoline proc

    ; 진입 시 RSP = pStackBase_ - 32 (코루틴 스택, 16-byte 정렬)
    ; CoCurrentCtx() — 인자 없음, shadow space만 필요
    sub     rsp,    32
    call    CoCurrentCtx
    add     rsp,    32

    ; rax = CoContext* (null이면 프로그래밍 오류 - unreachable)
    cmp     rax,    0
    jnz     CTX_OK
    int     3                           ; 코루틴 스택에서 컨텍스트를 찾지 못함
CTX_OK:

    ; 컨텍스트의 GS (TEB StackBase / StackLimit) 복원
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    mov     gs:[8],  r10
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    mov     gs:[16], r10

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
CoRun proc
    push    rbp
    push    rbx
    mov	    rbp,    rsp
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte

    ; 전달받은 인자 그대로 이어서 전달
    call    CoAllocCtx
    cmp     rax,    0
    je      FIN

    push	rax                     ; CoContext 포인터 백업 (컨텍스트 스택)

    lea     rbx,    YIELD           ; CoYield / 트램폴린에서 복귀할 때 사용할 주소
    mov     r10,    gs:[8]          ; 기존 StackBase 저장
    mov     r11,    gs:[16]         ; 기존 StackLimit 저장
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],     r10
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16],    r11
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

    mov     dword ptr[rax + OFFSET_COCTX_STATE],   csRun

    mov     r10,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKBASE]
    mov     r11,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKLIMIT]

    ; TEB에 커스텀 스택 범위 설정
    mov     gs:[8],     r10         ; StackBase  = 커스텀 스택 Base
    mov     gs:[16],    r11         ; StackLimit = 커스텀 스택 Limit (초기 커밋)

    ; 커스텀 스택으로 전환 후 fn() 호출
    ; call 대신 push-trampoline + jmp 패턴:
    ;   - fn() 정상 종료 시 CoFnEndTrampoline으로 복귀
    ;   - 마지막으로 재개한 컨텍스트(CoRun/CoResume)의 YIELD로 올바르게 점프
    ;   - 스택 레이아웃은 call과 동일 (push r11 + jmp = 호출 효과)
    mov     rsp,        r10             ; 코루틴 스택으로 전환 (RSP = pStackBase_, 0 mod 16)

    ; CoOnBeforeLaunch(CoContext*) — fn() 실행 직전
    ;   push rax  → RSP = pStackBase_-8  (8 mod 16)
    ;   sub  rsp, 40 (shadow 32 + align 8) → RSP = pStackBase_-48 (0 mod 16) ✓
    push    rax
    mov     rcx,        rax
    sub     rsp,        40
    call    CoOnBeforeLaunch
    add     rsp,        40
    pop     rax                         ; rax = CoContext* 복원, RSP = pStackBase_ (0 mod 16)

    sub     rsp,        32                  ; shadow space for fn()
    mov     r10,        [rax + OFFSET_COCTX_FN]
    lea     r11,        CoFnEndTrampoline
    push    r11                             ; "return address" = trampoline
    mov     rcx,        rax                 ; arg: CoContext* (rax는 push r11에 의해 불변)
    jmp     r10                             ; fn(CoContext*)

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


CoYield proc
    push    rbp
    push    rbx
    mov     rbp,    rsp
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte

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

    ; StackBase, StackLimit xchg
    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    xchg    r10,    gs:[8]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],  r10

    mov     r10,    [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    xchg    r10,    gs:[16]
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16], r10

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

    mov     dword ptr[rax + OFFSET_COCTX_STATE],              csYield
    jmp     rbx

FIN:
    mov     rsp,    rbp
    pop     rbx
    pop     rbp
    ret
CoYield endp

CoResume proc
    push    rbp
    push    rbx
    mov     rbp,    rsp
    sub     rsp,    8 + 32  ; align 8(push를 2번 했으므로.. 3번하면 16, 4번하면 24), shadow space for 32 byte

    cmp     rcx,    0
    jz      FIN

    push    rcx                     ; CoContext 포인터 백업 (컨텍스트 스택)
    mov     rax,    rcx

    ; StackBase 복원 및 백업
    mov     rbx,        gs:[8]
    xchg    rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    mov	    gs:[8],     rbx

    ; StackLimit 복원 및 백업
    mov     rbx,        gs:[16]
    xchg    rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    mov     gs:[16],    rbx

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
CoResume endp


end