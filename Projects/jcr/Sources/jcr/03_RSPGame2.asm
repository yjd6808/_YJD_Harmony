option casemap: NONE
include ../../jc/Sources/jc/_Extern/Extern.asm

; 가위바위보 게임
; 몇판할지 정한다.
; 

.data
    fmt_d   BYTE    "%d", 0
    fmt_s   BYTE    "%s", 0
    fmt_nl  BYTE    "%d\n", 0

.code

; RSPGame_main()
RSPGame_main proc
    mov     rcx,        0
    mov     rdx,        100
    call    Random_GenerateInt

    lea     rcx,        fmt_d
    mov     rdx,        rax
    call    Console_WriteLineWithArgs

    ret
RSPGame_main endp

end
