option casemap: NONE
include ../../jc/Sources/jc/_Extern/Extern.asm

extern CoAllocCtx       : proc
extern CoFreeCtx        : proc
extern CoFindCtxByAddr  : proc

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
    rip_    QWORD   ?
    rsp_    QWORD   ?
    rbp_    QWORD   ?

    gs8_    QWORD   ?
    gs16_   QWORD   ?

CoRegs ends

OFFSET_COREGS_RIP   EQU CoRegs.rip_
OFFSET_COREGS_RSP   EQU CoRegs.rsp_
OFFSET_COREGS_RBP   EQU CoRegs.rbp_
OFFSET_COREGS_GS8   EQU CoRegs.gs8_
OFFSET_COREGS_GS16  EQU CoRegs.gs16_

; ============================================================
CoContext struct 8
    id_         DWORD   ?
    threadId_   DWORD   ?
    regs_       CoRegs  <>
    stack_      CoStack <>
    state_      DWORD   ?
    _pad0_      DWORD   ?
    fn_         QWORD   ?
CoContext ends

OFFSET_COCTX_ID       EQU CoContext.id_
OFFSET_COCTX_THREADID EQU CoContext.threadId_
OFFSET_COCTX_REGS     EQU CoContext.regs_
OFFSET_COCTX_STACK    EQU CoContext.stack_
OFFSET_COCTX_STATE    EQU CoContext.state_
OFFSET_COCTX_FN       EQU CoContext.fn_

code
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

    push	rax                     ; CoContext 포인터 백업

    lea     rbx,    YIELD           ; 내부에서 CoYield로 복귀할 때 사용할 주소 저장해놓음
    mov     r10,    gs:[8]          ; 기존 StackBase 저장
    mov     r11,    gs:[16]         ; 기존 StackLimit 저장
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8],     r10
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16],    r11
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP],     rsp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP],     rbx
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP],     rbp

    mov     dword ptr[rax + OFFSET_COCTX_STATE],   csRun

    mov     r10,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKBASE]
    mov     r11,   [rax + OFFSET_COCTX_STACK + OFFSET_COSTACK_STACKLIMIT]

    ; TEB에 커스텀 스택 범위 설정
    mov     gs:[8],     r10         ; StackBase  = 커스텀 스택 Base
    mov     gs:[16],    r11         ; StackLimit = 커스텀 스택 Limit (초기 커밋)

    ; 커스텀 스택으로 전환 후 호출
    mov     rsp,        r10
    sub     rsp,        32              ; shadow space for 32 byte
    mov     rcx,        rax
    mov     rbx,        [rax + OFFSET_COCTX_FN]
    call    rbx

YIELD:
    ; 스택에 저장해놨던 CoContext 포인터 획득 (rcx)
    pop     rcx
    mov     ebx,        [rcx + OFFSET_COCTX_STATE]
    cmp     ebx,        csYield
    jz      FIN

    ; 내부에서 그냥 함수를 쭉 실행해버림. Yield 수행 자체를 안해버린 경우
    ; 이미 rcx에 CoContext 포인터가 있으므로 인자 설정되어있음
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

    mov     rcx,    rsp
    call    CoFindCtxByAddr
    cmp     rax,    0
    jz	    FIN

    ; 컨텍스트 스위치 수행
    ; 수행전 기존 컨텍스트 정보 백업

    ; StackBase, StackLimit 복원
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

    push    rcx                     ; CoContext 포인터 백업
    mov     rax,    rcx

    ; StackBase 복원 및 백업
    mov     rbx,        gs:[8]
    xchg    rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS8]
    mov	    gs:[8],     rbx

    ; StackBase 복원 및 백업
    mov     rbx,        gs:[16]       
    xchg    rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_GS16]
    mov     gs:[16],    rbx

    ; rbp 복구
    mov     rbx,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP]
    xchg    rbx,        rbp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RBP],     rbx

    ; rbp & rip 복구
    mov     r10,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP]
    mov     r11,        [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP]

    mov     dword ptr[rax + OFFSET_COCTX_STATE],   csRun
    lea     rbx,        YIELD           ; 내부에서 CoYield로 복귀할 때 사용할 주소 저장해놓음
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RSP],      rsp
    mov     [rax + OFFSET_COCTX_REGS + OFFSET_COREGS_RIP],      rbx

    ; 커스텀 스택으로 전환 후 호출
    mov     rsp,        r10
    jmp     r11
    
YIELD:
    ; 스택에 저장해놨던 CoContext 포인터 획득 (rcx)
    pop     rcx
    mov     ebx,        [rcx + OFFSET_COCTX_STATE]
    cmp     ebx,        csYield
    jz      FIN

    ; 내부에서 그냥 함수를 쭉 실행해버림. Yield 수행 자체를 안해버린 경우
    ; 이미 rcx에 CoContext 포인터가 있으므로 인자 설정되어있음
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

