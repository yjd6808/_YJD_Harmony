; ========================================================================
; 작성자 : 윤정도
; 코루틴 어셈블리 구현
; - CoSwitchImpl: 유일한 진입점. 순수 레지스터 교체 (분기 없음, 호출 없음)
; - CoEntryThunk: 첫 진입 thunk. regs_.rbp_ = ctx 규약
; - Windows x64 callee-saved 레지스터 전부 저장/복원
; ========================================================================

; [코루틴-09/11] 코루틴에 필요한 것만 직접 선언한다.
; - CoRun/CoYield/CoResume은 C++ 인라인 래퍼로 옮겨서 asm 진입점이 아니다.
OPTION CASEMAP: NONE
extern CoEntry          : proc      ; [코루틴-01] 코루틴 진입점

; TEB 오프셋 (참고용)
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
    _pad2_       BYTE    7 dup(?)
    pCommitLow_  QWORD   ?
CoStack ends

OFFSET_COSTACK_SIZE        EQU CoStack.size_
OFFSET_COSTACK_STACKBASE   EQU CoStack.pStackBase_   ; 높은 주소, 초기 RSP (TEB StackBase)
OFFSET_COSTACK_STACKEND    EQU CoStack.pStackEnd_    ; 예약 하단 (비상 패드 아래)
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
    _padAlign_ BYTE 8 dup(?)    ; offset 104  [코루틴-10] XMM 16 정렬 패딩

    ; Windows x64 callee-saved XMM 레지스터 (16 bytes each, 16-byte aligned)
    xmm6_   BYTE    16 dup(?)   ; offset 112
    xmm7_   BYTE    16 dup(?)   ; offset 128
    xmm8_   BYTE    16 dup(?)   ; offset 144
    xmm9_   BYTE    16 dup(?)   ; offset 160
    xmm10_  BYTE    16 dup(?)   ; offset 176
    xmm11_  BYTE    16 dup(?)   ; offset 192
    xmm12_  BYTE    16 dup(?)   ; offset 208
    xmm13_  BYTE    16 dup(?)   ; offset 224
    xmm14_  BYTE    16 dup(?)   ; offset 240
    xmm15_  BYTE    16 dup(?)   ; offset 256
    ; sizeof(CoRegs) = 272 (C++와 EQU로 일치. 어긋나면 09의 static_assert가 잡음)

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
    userData_   QWORD   ?           ; [코루틴-14] 사용자 포인터 (C++와 일치. asm은 안 씀)
    transfer_   QWORD   ?           ; [코루틴-14] 값 채널 (C++와 일치. asm은 안 씀)
    cancelRequested_ BYTE ?         ; [코루틴-14] 취소 요청 (C++와 일치. asm은 안 씀)
    _padEnd_    BYTE    7 dup(?)    ; + 취소 패딩 (C++와 일치)
    schedRegs_  CoRegs  <>          ; [코루틴-11] 스케줄러 레지스터 (C++와 일치)
CoContext ends

OFFSET_COCTX_ID       EQU CoContext.id_
OFFSET_COCTX_THREADID EQU CoContext.threadId_
OFFSET_COCTX_GEN      EQU CoContext.generation_
OFFSET_COCTX_REGS     EQU CoContext.regs_
OFFSET_COCTX_STACK    EQU CoContext.stack_
OFFSET_COCTX_STATE    EQU CoContext.state_
OFFSET_COCTX_FN       EQU CoContext.fn_
OFFSET_COCTX_CALLER   EQU CoContext.callerCtx_
OFFSET_COCTX_SCHEDREGS EQU CoContext.schedRegs_

code

; ============================================================
; CoSwitchImpl
;   [코루틴-11] 유일한 asm 진입점. 순수 레지스터 교체 (분기 없음, 호출 없음)
;   - rcx = 저장할 CoRegs*, rdx = 읽을 const CoRegs*
;   - 현재 레지스터를 save에 저장하고 load에서 복원한 뒤 ret로 복귀한다.
;   - rbx는 스택에 잠시 얹는다. (양쪽 스택이 각자 보관. ctx 필드 불필요)
;   - RIP은 [rsp]로 오간다. (CoRegs.rip_ 필드는 레이아웃 호환용으로 유지)
;   - TEB 3종 + MXCSR/x87 포함. gs16 한계 동기화(03)는 C++ 래퍼가 한다.
; ============================================================
CoSwitchImpl proc FRAME
    .endprolog
    ; --- save (현재 상태) ---
    push    rbx
    mov     [rcx + OFFSET_COREGS_RSP],  rsp
    mov     [rcx + OFFSET_COREGS_RBP],  rbp
    mov     [rcx + OFFSET_COREGS_RSI],  rsi
    mov     [rcx + OFFSET_COREGS_RDI],  rdi
    mov     [rcx + OFFSET_COREGS_R12],  r12
    mov     [rcx + OFFSET_COREGS_R13],  r13
    mov     [rcx + OFFSET_COREGS_R14],  r14
    mov     [rcx + OFFSET_COREGS_R15],  r15
    movaps  [rcx + OFFSET_COREGS_XMM6],  xmm6
    movaps  [rcx + OFFSET_COREGS_XMM7],  xmm7
    movaps  [rcx + OFFSET_COREGS_XMM8],  xmm8
    movaps  [rcx + OFFSET_COREGS_XMM9],  xmm9
    movaps  [rcx + OFFSET_COREGS_XMM10], xmm10
    movaps  [rcx + OFFSET_COREGS_XMM11], xmm11
    movaps  [rcx + OFFSET_COREGS_XMM12], xmm12
    movaps  [rcx + OFFSET_COREGS_XMM13], xmm13
    movaps  [rcx + OFFSET_COREGS_XMM14], xmm14
    movaps  [rcx + OFFSET_COREGS_XMM15], xmm15
    stmxcsr [rcx + OFFSET_COREGS_MXCSR]
    fnstcw  [rcx + OFFSET_COREGS_FPUCW]
    mov     r10,    gs:[8]
    mov     [rcx + OFFSET_COREGS_GS8],   r10
    mov     r10,    gs:[16]
    mov     [rcx + OFFSET_COREGS_GS16],  r10
    mov     r10,    gs:[TEB_DEALLOCATION_STACK]
    mov     [rcx + OFFSET_COREGS_GS1478], r10
    ; --- load ---
    mov     r10,    [rdx + OFFSET_COREGS_GS8]
    mov     gs:[8],     r10
    mov     r10,    [rdx + OFFSET_COREGS_GS16]
    mov     gs:[16],    r10
    mov     r10,    [rdx + OFFSET_COREGS_GS1478]
    mov     gs:[TEB_DEALLOCATION_STACK], r10
    ldmxcsr [rdx + OFFSET_COREGS_MXCSR]
    fldcw   [rdx + OFFSET_COREGS_FPUCW]
    movaps  xmm6,   [rdx + OFFSET_COREGS_XMM6]
    movaps  xmm7,   [rdx + OFFSET_COREGS_XMM7]
    movaps  xmm8,   [rdx + OFFSET_COREGS_XMM8]
    movaps  xmm9,   [rdx + OFFSET_COREGS_XMM9]
    movaps  xmm10,  [rdx + OFFSET_COREGS_XMM10]
    movaps  xmm11,  [rdx + OFFSET_COREGS_XMM11]
    movaps  xmm12,  [rdx + OFFSET_COREGS_XMM12]
    movaps  xmm13,  [rdx + OFFSET_COREGS_XMM13]
    movaps  xmm14,  [rdx + OFFSET_COREGS_XMM14]
    movaps  xmm15,  [rdx + OFFSET_COREGS_XMM15]
    mov     r12,    [rdx + OFFSET_COREGS_R12]
    mov     r13,    [rdx + OFFSET_COREGS_R13]
    mov     r14,    [rdx + OFFSET_COREGS_R14]
    mov     r15,    [rdx + OFFSET_COREGS_R15]
    mov     rdi,    [rdx + OFFSET_COREGS_RDI]
    mov     rsi,    [rdx + OFFSET_COREGS_RSI]
    mov     rbp,    [rdx + OFFSET_COREGS_RBP]
    mov     rsp,    [rdx + OFFSET_COREGS_RSP]
    pop     rbx
    ret                                          ; [rsp] = 저장 당시의 반환주소
CoSwitchImpl endp

; ============================================================
; CoEntryThunk
;   [코루틴-11] 첫 진입 thunk. regs_.rbp_ = ctx 규약으로 CoEntry를 호출한다.
;   - CoRun 준비: RSP = pStackBase_-16, [base-16] = thunk 주소, [base-8] = 0.
;   - ret로 진입하면 RSP = base-8 (8 mod 16, call 규약 OK).
;   - .allocstack 40 뒤 반환주소는 [base-8] = 0이라 언와인더가 멈춘다. (01 유지)
;   - CoEntry는 끝나면 스위치아웃하므로 이 thunk로 복귀하지 않는다. (int 3)
; ============================================================
CoEntryThunk proc FRAME
    sub     rsp,    40
    .allocstack 40
    .endprolog
    mov     rcx,    rbp             ; ctx (CoRun이 regs_.rbp_에 넣어둠)
    call    CoEntry
    int     3                       ; 복귀 불가
CoEntryThunk endp


end
