option casemap: NONE
include ../../jc/Sources/jc/_Extern/Extern.asm

RoundPlayInfo struct
    playerVal       DWORD       ?
    comVal          DWORD       ?
RoundPlayInfo ends

; 미리 초기화 해놔야함.
; 아래 섹션에서 RoundPlayInfo.playerVal 이렇게 접근이 안됨.
OFFSET_PLAYER_VAL   EQU RoundPlayInfo.playerVal
OFFSET_COM_VAL      EQU RoundPlayInfo.comVal


; 가위바위보 게임
; 몇판할지 정한다.
.data
    fmt_d                   BYTE    "%d", 0
    fmt_s                   BYTE    "%s", 0
    fmt_nl                  BYTE    "%d", 10, 0
    str_round_input_header  BYTE    "Input Round> ", 0
    str_round_input_size    DWORD   256   
    str_round_input         BYTE    256 DUP(0)

    fmt_round_start         BYTE    "<%d ROUND>", 10, 0
    str_player_input_header BYTE    "Scissor: 1, Rock: 2, Paper: 3", 10, "Player Input>", 0
    str_player_input_size   DWORD   10
    str_player_input        BYTE    10  DUP(0)
    fmt_computer_input      BYTE    "Computer Input>%d", 0
    fmt_vs                  BYTE    "%s vs %s", 0

    win_board               QWORD   0   ; _u8 win_board[round_cnt] 플레이어가 이기면 1, 지면 -1, 무승부면 0 / 포인터, 라운드 수만큼 존재, RoundPlayInfo에 포함되는게 옳지만 학습을 위해 분리함
    history_board           QWORD   0   ; RoundPlayInfo history_board[round] 플레이어가 낸 것, 컴퓨터가 낸 것 기록

    round                   DWORD   0
    round_cnt               DWORD   0

    str_s       BYTE "Scissor", 0    ;   1
    str_r       BYTE "Rock", 0    ;   2
    str_p       BYTE "Paper", 0     ;   3
    str_rsp_arr QWORD 0, OFFSET str_s, OFFSET str_r, OFFSET str_p; 가위바위보 인덱스 접근을 위해 0번인덱스는 null로 둠

    obj RoundPlayInfo <0, 0>
.code

;; /////////////////////////////////////////////////////////////////////
RSPGame_main proc
    push    rbx
    push    rbp                 ; 스택 16바이트 정렬된 상태. 
    mov     rbp,    rsp
    sub     rsp,    40          ; rbx를 push 했으므로 32가 아닌 40을 sub함.

    ; 라운드 수 입력받기
    lea     rcx,    [str_round_input_header]
    lea     rdx,    [str_round_input]
    mov     r8d,    dword ptr[str_round_input_size]
    call    Console_ReadLineBuffered    ; 콜 직전엔 무조건 rsp % 16 = 0을 맞춰줘야함.

    ; 라운드 수 텍스트를 숫자로 변환
    lea     rcx,    [str_round_input]
    call    StringUtil_ToNumber_U32
    mov     dword ptr[round_cnt],   eax

    add     eax,    1
    mov     ecx,                    eax; ecx를 초기화해도 rcx의 상위 4바이트는 0으로 초기화된다. 50라운드면 51바이트 할당 라운드 인덱스 접근을 위함
    call    Memory_Allocate
    mov     [win_board],            rax

    mov     rcx,                    [win_board]
    mov     edx,                    dword ptr[round_cnt]
    mov     r8b,                    0
    call    Memory_Set

    mov     ecx,                    dword ptr[round_cnt]
    add     ecx,                    1
    imul    ecx,                    SIZEOF RoundPlayInfo
    call    Memory_AllocateWithZero
    mov     [history_board],        rax

    ; 1라운드로 초기화
    mov     eax,    1
    mov     dword ptr[round],       eax
GAME_LOOP:
    call    PlayRound
    call    PrintHistory

    inc     dword ptr[round]
    mov     eax,    dword ptr[round]
    cmp     eax,    dword ptr[round_cnt]
    jle     GAME_LOOP

GAME_END:

    mov     rcx,    [win_board]
    call    Memory_Deallocate

    mov     rcx,    [history_board]
    call    Memory_Deallocate

    mov     rsp,    rbp         ;   rsp 복구
    pop     rbp
    pop     rbx
    ret
RSPGame_main endp


;; /////////////////////////////////////////////////////////////////////
;; 승부를 가린다.
;;      가위: 1, 바위:2, 보:3
;; 보:0 가위: 1, 바위:2, 보:3, 가위:4
;; 즉, 자기보다 높은 녀석한테는 지고 낮은 대상한텐 이김

CompareRSP proc
    push    rbp
    mov     rbp,    rsp
    sub     rsp,    32

    playerVal      TEXTEQU <rbp - 8>
    comVal         TEXTEQU <rbp - 16>

    mov     dword ptr[playerVal],   ecx
    mov     dword ptr[comVal],   edx

    ; 플레이어 - 컴퓨터 == 2인 경우 컴퓨터를 3만큼 더해준다.
    sub     ecx,    edx
    cmp     ecx,    2
    jnz     L_0 ; -2일 수도있으니 한번더 검사

    add     dword ptr[comVal],  3
    jmp     COMP

L_0:
    ; 플레이어 - 컴퓨터 == -2인 경우 컴퓨터를 -3만큼 빼준다.
    cmp     ecx,    -2
    jnz     COMP

    sub     dword ptr[comVal], 3

COMP:
    mov     ecx,    dword ptr[playerVal]
    sub     ecx,    dword ptr[comVal]
    cmp     ecx,    0
    jz      DRAW
    cmp     ecx,    1
    jz      WIN
    cmp     ecx,    -1
    jz      LOSE
    jmp     END_
WIN:
    mov     rax,    1
    jmp     END_

DRAW:
    mov     rax,    0
    jmp     END_

LOSE:
    mov     rax,    -1

END_:
    mov     rsp,    rbp
    pop     rbp
    ret

CompareRSP endp



;; /////////////////////////////////////////////////////////////////////
.const
    result_str1         BYTE "YOU LOSE", 0      ;  -1
    result_str2         BYTE "YOU DRAW", 0      ;  0
    result_str3         BYTE "YOU WIN", 0       ;  1
    invalid_input_str   BYTE "INVALID INPUT", 0
    result_str_arr QWORD OFFSET result_str1, OFFSET result_str2, OFFSET result_str3

.code
PlayRound proc
    push    rbx
    push    rbp
    mov     rbp,        rsp
    sub     rsp,        40 + 32     ; shadow space 할당 (callee 함수(이 함수에서 호출한 함수) 에서 해당 스택에 접근하기 때문)

    ; 로컬 변수
    playerInput         TEXTEQU <rbp - 8>
    computerInput       TEXTEQU <rbp - 16>

    ; 몇라운드인지 출력
    lea     rcx,        [fmt_round_start]
    mov     edx,        dword ptr[round]
    call    Console_WriteFmt

P_INPUT:
    ; 플레이어 입력 받기
    lea     rcx,        [str_player_input_header]
    lea     rdx,        [str_player_input]
    mov     r8d,        [str_player_input_size]
    call    Console_ReadLineBuffered

    ; 플레이어 입력 숫자로 변환 후 로컬 변수에 저장
    lea     rcx,        [str_player_input]
    call    StringUtil_ToNumber_U32
    cmp     rax,        3
    jg      P_INVALID
    cmp     rax,        1
    jl      P_INVALID

    mov     dword ptr[playerInput], eax
    jmp     P_INPUT_END
P_INVALID:
    lea     rcx,        [invalid_input_str]
    call    Console_WriteLine
    jmp     P_INPUT

P_INPUT_END:

    ; 컴퓨터 입력 뽑기
    mov     ecx,        1
    mov     edx,        3
    call    Random_GenerateInt
    mov     dword ptr[computerInput], eax

    lea     rcx,        [fmt_computer_input]
    mov     edx,        eax
    call    Console_WriteLineFmt

    ; 입력 기록
    mov     rcx,        [history_board]
    mov     edx,        [round]
    imul    edx,        sizeof RoundPlayInfo
    
    add     rcx,        rdx

    mov     eax,        dword ptr[playerInput]
    mov     ebx,        dword ptr[computerInput]
    mov     dword ptr [rcx + OFFSET_PLAYER_VAL],  eax
    mov     dword ptr [rcx + OFFSET_COM_VAL],     ebx
    
    ; %s vs %s 출력
    lea     rcx,        [fmt_vs]
    
    mov     eax,        dword ptr[playerInput]
    lea     r8,         [str_rsp_arr]
    mov     rdx,        [r8 + 8 * rax]

    mov     eax,        dword ptr[computerInput]
    lea     r9,         [str_rsp_arr]
    mov     r8,         [r9 + 8 * rax]

    call    Console_WriteLineFmt

    ; 승자 가리기
    ; 가위1, 바위2, 보3
    mov     ecx,        dword ptr[playerInput]
    mov     edx,        dword ptr[computerInput]
    call    CompareRSP

    ; 승부 기록
    mov     rcx,        [win_board]
    mov     edx,        [round]
    mov     byte ptr[rcx + rdx],    al

    ; 승부 정보 출력
    add     rax,        1       ; -1, 0, 1 -> 0, 1, 2로 변경
    lea     r8,         [result_str_arr]
    mov     rcx,        [r8 + rax * 8]
    call    Console_WriteLine

    mov     rsp,        rbp
    pop     rbp
    pop     rbx
    ret
PlayRound endp

;; /////////////////////////////////////////////////////////////////////
.const
    str_column_round    BYTE    "ROUND", 0
    str_column_win      BYTE    "RESULT", 0
    str_column_player   BYTE    "PLAYER", 0
    str_column_com      BYTE    "COM", 0
    fmt_history1        BYTE    "%7s%10s%15s%15s", 0
    fmt_history2        BYTE    "%7d%10s%15s%15s", 0
.code

PrintHistory proc
    push    rbx
    push    rbp
    mov     rbp,    rsp
    sub     rsp,    64 + 8 + 32

    winResult      TEXTEQU <rbp - 8>
    playerResult   TEXTEQU <rbp - 16>
    comResult      TEXTEQU <rbp - 24>

    pWinStr        TEXTEQU <rbp - 32>
    pPlayerStr     TEXTEQU <rbp - 40>
    pComStr        TEXTEQU <rbp - 48>

    i              TEXTEQU <rbp - 56>

    lea     rcx,    [fmt_history1]
    lea     rdx,    [str_column_round]
    lea     r8,     [str_column_win]
    lea     r9,     [str_column_player]

    lea     rax,    [str_column_com]
    mov     [rsp + 32], rax
    call    Console_WriteLineFmt

    mov     dword ptr[i],    1
    mov     ebx,    dword ptr[round]

L:
    mov     eax,    dword ptr[i]

    ; win_board에서 승부 결과 가져오기
    mov     rcx,    [win_board]
    mov     rdx,    0
    mov     dl,     byte ptr[rcx + rax]
    mov     dword ptr[winResult], edx

    ; history_board에서 플레이어/컴퓨터 입력 결과 가져오기
    mov     rcx,    [history_board]
    lea     rdx,    [rcx + rax * SIZEOF RoundPlayInfo]

    mov     ecx,    dword ptr[rdx + OFFSET_PLAYER_VAL]
    mov     r8d,    dword ptr[rdx + OFFSET_COM_VAL]

    mov     dword ptr[playerResult],    ecx
    mov     dword ptr[comResult],       r8d

    ; 승부 결과 문자열 포인터 저장
    mov     r8,     0
    mov     r8b,    byte ptr[winResult]
    add     r8b,    1   ; -1 0 1 -> 0 1 2
    lea     rcx,    [result_str_arr]
    mov     rdx,    [rcx + r8 * 8] ; 포인터 크기 8
    mov     [pWinStr],  rdx

    ; 플레이어 입력결과 문자열 포인터 저장
    mov     r8d,    dword ptr[playerResult]
    lea     rcx,    [str_rsp_arr]
    mov     rcx,    [rcx + r8 * 8]
    mov     [pPlayerStr], rcx

    ; 컴퓨터 입력결과 문자열 포인터 저장
    mov     r8d,    dword ptr[comResult]
    lea     rcx,    [str_rsp_arr]
    mov     rdx,    [rcx + r8 * 8]
    mov     [pComStr], rdx

    ; 결과 출력
    lea     rcx,    [fmt_history2]
    mov     edx,    eax
    mov     r8,     [pWinStr]
    mov     r9,     [pPlayerStr]
    mov     r10,    [pComStr]
    mov     [rsp + 32], r10
    call    Console_WriteLineFmt

    cmp     dword ptr[i],  ebx
    jge     E

    inc     dword ptr[i]
    jmp     L

E:

    mov     rsp,    rbp
    pop     rbp
    pop     rbx
    ret 
PrintHistory endp

end
