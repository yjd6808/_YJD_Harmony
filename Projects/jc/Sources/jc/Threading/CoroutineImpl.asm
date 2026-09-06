; ========================================================================
; ?묒꽦??: ?ㅼ젙??
; 肄붾（???댁뀍釉붾━ 援ы쁽
; - CoSwitchImpl: ?좎씪??吏꾩엯?? ?쒖닔 ?덉??ㅽ꽣 援먯껜 (遺꾧린 ?놁쓬, ?몄텧 ?놁쓬)
; - CoEntryThunk: 泥?吏꾩엯 thunk. regs_.rbp_ = ctx 洹쒖빟
; - Windows x64 callee-saved ?덉??ㅽ꽣 ?꾨? ???蹂듭썝
; ========================================================================

; [肄붾（??09/11] 肄붾（?댁뿉 ?꾩슂??寃껊쭔 吏곸젒 ?좎뼵?쒕떎.
; - CoRun/CoYield/CoResume? C++ ?몃씪???섑띁濡???꺼??asm 吏꾩엯?먯씠 ?꾨땲??
OPTION CASEMAP: NONE
extern CoEntry          : proc      ; [肄붾（??01] 肄붾（??吏꾩엯??

; TEB ?ㅽ봽??(李멸퀬?? DeallocationStack? 援먯껜?섏? ?딆쓬. [肄붾（??02])
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
    magic_       QWORD   ?           ; [肄붾（??05] ?ㅽ깮 ?앸퀎 留ㅼ쭅 (C++ CoStack怨??쒖꽌/?ш린 ?쇱튂)
    pEmergencyTop_ QWORD ?           ; [肄붾（??04] 鍮꾩긽 諛대뱶 ?곷떒 (C++? ?쇱튂)
    pReserveBase_ QWORD  ?           ; [肄붾（??04] ?덉빟 ?쒖옉 二쇱냼 (C++? ?쇱튂)
    overflowed_  BYTE    ?           ; [肄붾（??04] ?ㅻ쾭?뚮줈??泥섎━ ?щ? (C++? ?쇱튂)
    _pad2_       BYTE    7 dup(?)
    pCommitLow_  QWORD   ?
CoStack ends

OFFSET_COSTACK_SIZE        EQU CoStack.size_
OFFSET_COSTACK_STACKBASE   EQU CoStack.pStackBase_   ; ?믪? 二쇱냼, 珥덇린 RSP (TEB StackBase)
OFFSET_COSTACK_STACKEND    EQU CoStack.pStackEnd_    ; ?덉빟 ?섎떒 (鍮꾩긽 ?⑤뱶 ?꾨옒)
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
    gs1478_ QWORD   ?           ; offset  40  TEB DeallocationStack: 0x1478 [肄붾（??02/04]

    ; Windows x64 callee-saved ?뺤닔 ?덉??ㅽ꽣
    rsi_    QWORD   ?           ; offset  48
    rdi_    QWORD   ?           ; offset  56
    r12_    QWORD   ?           ; offset  64
    r13_    QWORD   ?           ; offset  72
    r14_    QWORD   ?           ; offset  80
    r15_    QWORD   ?           ; offset  88

    ; [肄붾（??08] 遺?숈냼?섏젏 ?쒖뼱 ?곹깭 (C++ CoRegs? ?쒖꽌/?ш린 ?쇱튂)
    mxcsr_  DWORD   ?           ; offset  96
    fpucw_  WORD    ?           ; offset 100
    _padFp_ WORD    ?           ; offset 102
    _padAlign_ BYTE 8 dup(?)    ; offset 104  [肄붾（??10] XMM 16 ?뺣젹 ?⑤뵫

    ; Windows x64 callee-saved XMM ?덉??ㅽ꽣 (16 bytes each, 16-byte aligned)
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
    ; sizeof(CoRegs) = 272 (C++? EQU濡??쇱튂. ?닿툔?섎㈃ 09??static_assert媛 ?≪쓬)

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
    generation_ DWORD   ?           ; [肄붾（??05] ?몃? 踰덊샇 (C++ CoContext? ?쒖꽌/?ш린 ?쇱튂)
    _padGen_    DWORD   ?
    regs_       CoRegs  <>
    stack_      CoStack <>
    state_      DWORD   ?
    _pad0_      DWORD   ?
    fn_         QWORD   ?
    callerCtx_  QWORD   ?
    userData_   QWORD   ?           ; [肄붾（??14] ?ъ슜???ъ씤??(C++? ?쇱튂. asm? ???)
    transfer_   QWORD   ?           ; [肄붾（??14] 媛?梨꾨꼸 (C++? ?쇱튂. asm? ???)
    cancelRequested_ BYTE ?         ; [肄붾（??14] 痍⑥냼 ?붿껌 (C++? ?쇱튂. asm? ???)
    _padEnd_    BYTE    7 dup(?)    ; + 痍⑥냼 ?⑤뵫 (C++? ?쇱튂)
    schedRegs_  CoRegs  <>          ; [肄붾（??11] ?ㅼ?以꾨윭 ?덉??ㅽ꽣 (C++? ?쇱튂)
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
;   [肄붾（??11] ?좎씪??asm 吏꾩엯?? ?쒖닔 ?덉??ㅽ꽣 援먯껜 (遺꾧린 ?놁쓬, ?몄텧 ?놁쓬)
;   - rcx = ??ν븷 CoRegs*, rdx = ?쎌쓣 const CoRegs*
;   - ?꾩옱 ?덉??ㅽ꽣瑜?save????ν븯怨?load?먯꽌 蹂듭썝????ret濡?蹂듦??쒕떎.
;   - rbx???ㅽ깮???좎떆 ?밸뒗?? (?묒そ ?ㅽ깮??媛곸옄 蹂닿?. ctx ?꾨뱶 遺덊븘??
;   - RIP? [rsp]濡??ㅺ컙?? (CoRegs.rip_ ?꾨뱶???덉씠?꾩썐 ?명솚?⑹쑝濡??좎?)
;   - TEB 3醫?+ MXCSR/x87 ?ы븿. gs16 ?쒓퀎 ?숆린??03)??C++ ?섑띁媛 ?쒕떎.
; ============================================================
CoSwitchImpl proc FRAME
    .endprolog
    ; --- save (?꾩옱 ?곹깭) ---
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
    ret                                          ; [rsp] = ????뱀떆??諛섑솚二쇱냼
CoSwitchImpl endp

; ============================================================
; CoEntryThunk
;   [肄붾（??11] 泥?吏꾩엯 thunk. regs_.rbp_ = ctx 洹쒖빟?쇰줈 CoEntry瑜??몄텧?쒕떎.
;   - CoRun 以鍮? RSP = pStackBase_-16, [base-16] = thunk 二쇱냼, [base-8] = 0.
;   - ret濡?吏꾩엯?섎㈃ RSP = base-8 (8 mod 16, call 洹쒖빟 OK).
;   - .allocstack 40 ??諛섑솚二쇱냼??[base-8] = 0?대씪 ?몄??몃뜑媛 硫덉텣?? (01 ?좎?)
;   - CoEntry???앸굹硫??ㅼ쐞移섏븘?껎븯誘濡???thunk濡?蹂듦??섏? ?딅뒗?? (int 3)
; ============================================================
CoEntryThunk proc FRAME
    sub     rsp,    40
    .allocstack 40
    .endprolog
    mov     rcx,    rbp             ; ctx (CoRun??regs_.rbp_???ｌ뼱??
    call    CoEntry
    int     3                       ; 蹂듦? 遺덇?
CoEntryThunk endp


end
