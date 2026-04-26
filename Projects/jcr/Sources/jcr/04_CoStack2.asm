option casemap: NONE
include ../../jc/Sources/jc/_Extern/Extern.asm

.data

.code

;; /////////////////////////////////////////////////////////////////////
co_stack_test proc
    push    rbp
    mov     rbp,    rsp

    ; x64 기준 __chkstk 함수 내부 구조
    ; 함수가 아닌 asm 코드로 작성되어있다.
    ; x86 버전 코드는 chkstk.asm으로 존재한다.
    ; Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.29.30133\crt\src\i386\chkstk.asm
    ; 이 코드를 c코드로 보면 이런 느낌임
    ; 단순하게 memory touch 후 커널에서 스택이 페이지 가드인지 체크해서 확장하는 방식
    ; void __chkstk(size_t size)
    ; {
    ;     save(ecx);
    ; 
    ;     uintptr_t new_tos = current_esp - size;
    ; 
    ;     if (overflow_occurred)
    ;         new_tos = 0;
    ; 
    ;     uintptr_t page = align_down(current_esp, PAGE_SIZE);
    ; 
    ;     while (new_tos < page)
    ;     {
    ;         page -= PAGE_SIZE;
    ;         *(volatile int*)page;   // probe
    ;     }
    ; 
    ;     esp = new_tos;
    ;     restore(ecx);
    ;     move_return_address_to_new_stack();
    ;     return;
    ; }

    ; x64 코드도 구조는 동일함.
    ; 단 TEB의 StackBase와 StackLimit을 이용해서 스택 범위를 체크하는 방식으로 구현되어있다.
    ; 따라서 나도 TEB의 StackBase와 StackLimit을 초기화 해둬야 한다.

    ; 0000000140229310  sub         rsp,10h
    ; 0000000140229314  mov         qword ptr [rsp],r10
    ; 0000000140229318  mov         qword ptr [rsp+8],r11
    ; 000000014022931D  xor         r11,r11
    ; 0000000140229320  lea         r10,[rsp+18h]
    ; 0000000140229325  sub         r10,rax
    ; 0000000140229328  cmovb       r10,r11
    ; 000000014022932C  mov         r11,qword ptr gs:[10h]
    ; 0000000140229335  cmp         r10,r11
    ; 0000000140229338  jae         0000000140229350
    ; 000000014022933A  and         r10w,0F000h
    ; 0000000140229340  lea         r11,[r11-1000h]
    ; 0000000140229347  mov         byte ptr [r11],0
    ; 000000014022934B  cmp         r10,r11
    ; 000000014022934E  jne         0000000140229340
    ; 0000000140229350  mov         r10,qword ptr [rsp]
    ; 0000000140229354  mov         r11,qword ptr [rsp+8]
    ; 0000000140229359  add         rsp,10h
    ; 000000014022935D  ret


    ; 현재 TEB 스택 범위 저장 (GS:[8]=StackBase, GS:[16]=StackLimit)
    mov     rax,    gs:[8]
    push    rax                     ; 기존 StackBase 저장
    mov     rax,    gs:[16]
    push    rax                     ; 기존 StackLimit 저장

    ; TEB에 커스텀 스택 범위 설정
    mov     gs:[8],     rdx         ; StackBase  = 커스텀 스택 Top
    mov     rax,        rdx
    sub     rax,        1000h       ; StackLimit = Top - 1 pages (초기 커밋)
    mov     gs:[16],    rax

    ; 커스텀 스택으로 전환 후 호출
    mov     rax,    rcx
    mov     rsp,    rdx
    sub     rsp,    20h
    call    rax

    ; TEB 복원
    pop     rax
    mov     gs:[16],    rax
    pop     rax
    mov     gs:[8],     rax

    mov     rsp,    rbp
    pop     rbp
    ret
co_stack_test endp


end
