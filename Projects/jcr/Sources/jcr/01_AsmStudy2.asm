; 작성자: 윤정도
; 작성일: 5/18/2024 12:34:38 PM
; ==============================
; 참고: https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170 : [x64 calling convention]
; 참고: https://www.felixcloutier.com/x86/ : [x86/x64 instruction set]
OPTION CASEMAP: NONE

.data
szHelloWorld			db			"Hello World!", 0

.code

; 1. abs_i64(_s64 x)
; 입력된 정수 x의 절댓값을 반환한다.
abs_i64 proc
    mov     rax,        rcx
    cmp     rax,        0
    jge     _E
    neg     rax
_E:
    ret
abs_i64 endp



; 2. max_i64(_s64 a, _s64 b)
; a와 b 중 더 큰 값을 반환한다.
max_i64 proc

    mov     rax,        rcx
    cmp     rax,        rdx
    jg      _E
    mov     rax,        rdx

_E:
    ret

max_i64 endp


; 3. min_i64(_s64 a, _s64 b)
; a와 b 중 더 작은 값을 반환한다.
min_i64 proc
    mov     rax,        rcx
    cmp     rax,        rdx
    jl      _E
    mov     rax,        rdx
_E:
    ret
min_i64 endp


; 4. clamp_i64(_s64 x, _s64 lo, _s64 hi)
; x를 [lo, hi] 범위로 clamp하여 반환한다.
clamp_i64 proc
    mov     rax,    rcx

    cmp     rdx,    r8
    jle     _L
    xchg    rdx,    r8

_L:
    cmp     rax,    rdx
    jg      _H
    mov     rax,    rdx
    jmp     _E

_H:
    cmp     rax,    r8
    jl      _E
    mov     rax,    r8

_E:
    ret
clamp_i64 endp


; 5. factorial_u64(_u64 n)
; n! (1 * 2 * ... * n)을 계산하여 반환한다. (0! = 1)

factorial_u64 proc

        cmp     rcx,    1
        ja      _START

        mov     rax,    1
        jmp     _END

_START: 
        mov     rdx,    1
        mov     rax,    1

_LOOP:  
        cmp     rdx,    rcx
        ja      _END
        imul    rax,    rdx
        inc     rdx
        jmp     _LOOP

_END:   ret
factorial_u64 endp


; 6. count_bits_u64(_u64 x)
; x의 이진수 표현에서 1의 개수를 반환한다(비트 카운트).
count_bits_u64 proc
    ; TODO:
    ; shift/AND 또는 bt 등을 사용하여 1의 개수를 세기


    push    rbp
    push    rbx
    mov     rbp,    rsp
    sub     rsp,    8

    mov     rax,    0
    mov     rbx,    0
    mov     qword ptr[rbp - 8],  1

_START:     
    cmp     rax,    64
    jge     _END

    mov     rdx,    0
    test    rcx,    1
    cmovne  rdx,    qword ptr[rbp - 8]
    shr     rcx,    1
    add     rbx,    rdx
    inc     rax
    jmp     _START

_END:   
    mov     rax,    rbx
    mov     rsp,    rbp
    pop     rbx
    pop     rbp
    ret
count_bits_u64 endp


; 7. flip_bits_u64(_u64 x)
; 모든 비트를 반전시킨 값을 반환한다.

flip_bits_u64 proc
    ; TODO:
    ; NOT 또는 XOR 등을 이용하여 비트를 반전

    mov     rax,        -1
    xor     rcx,        rax
    mov     rax,        rcx
    ret


flip_bits_u64 endp


; 7. reverse_bits_u64(_u64 x)
; 64비트의 순서를 뒤집는다 (LSB <-> MSB).
reverse_bits_u64 proc
    ; TODO:
    ; 양쪽 끝 비트를 서로 교환하는 방식으로 구현

    ; i = 0 ~ 31 - 
    ; v = 63 - i = 63 ~ 32
    ; 
    ; _u64 tempi = x & (1 << i);
    ; _u64 tempv = x & (1 << v);
    ; x &= ~(1 << i)
    ; x &= ~(1 << v)
    ; if (tempi)
    ;   x |= 1 << v
    ; if (tempv)
    ;   x |= 1 << i

    push    rbp
    push    rbx
    mov     rbp,        rsp
    sub     rsp,        72

    mov     qword ptr[rbp - 8],         0   ;   i
    mov     qword ptr[rbp - 16],        0   ;   v
    mov     qword ptr[rbp - 24],        0   ;   tempi
    mov     qword ptr[rbp - 32],        0   ;   tempv
    mov     qword ptr[rbp - 40],        0   ;   i << i
    mov     qword ptr[rbp - 48],        0   ;   i << v
    mov     qword ptr[rbp - 56],        63  ;   63
    mov     qword ptr[rbp - 64],        rcx ;   x

L_:
    cmp     qword ptr[rbp - 8],         32
    jge     E_

    ; v = 63 - i
    mov     rax,                        qword ptr[rbp - 56]
    sub     rax,                        qword ptr[rbp - 8]
    mov     qword ptr[rbp - 16],        rax

    ; 1 << i
    mov     rax,                        1
    mov     cl,                         byte ptr[rbp - 8]
    shl     rax,                        cl
    mov     qword ptr[rbp - 40],        rax
    
    ; i << v
    mov     rax,                        1
    mov     cl,                         byte ptr[rbp - 16]
    shl     rax,                        cl
    mov     qword ptr[rbp - 48],        rax

    ; tempi = x & (1 << i);
    mov     rax,                        qword ptr[rbp - 64]
    and     rax,                        qword ptr[rbp - 40]
    mov     qword ptr[rbp - 24],        rax

    ; tempv = x & (1 << v);
    mov     rax,                        qword ptr[rbp - 64]
    and     rax,                        qword ptr[rbp - 48]
    mov     qword ptr[rbp - 32],        rax

    ; x &= ~(1 << i)
    mov     rax,                        qword ptr[rbp - 40]
    not     rax
    and     qword ptr[rbp - 64],        rax

    ; x &= ~(1 << v)
    mov     rax,                        qword ptr[rbp - 48]
    not     rax
    and     qword ptr[rbp - 64],        rax

    ; if (tempi)
    ;   x |= 1 << v
    mov     rax,                        qword ptr[rbp - 24]
    test    rax,                        rax
    jz      J_1
    mov     rax,                        qword ptr[rbp - 64]
    or      rax,                        qword ptr[rbp - 48]
    mov     qword ptr[rbp - 64],        rax

J_1:
    ; if (tempv)
    ;   x |= 1 << i
    mov     rax,                        qword ptr[rbp - 32]
    test    rax,                        rax
    jz      J_2
    mov     rax,                        qword ptr[rbp - 64]
    or      rax,                        qword ptr[rbp - 40]
    mov     qword ptr[rbp - 64],        rax

J_2:
    inc     qword ptr[rbp - 8]
    jmp     L_

E_:
    mov     rax,        qword ptr[rbp - 64]
    mov     rsp,        rbp
    pop     rbx
    pop     rbp
    ret

reverse_bits_u64 endp


; 8. sum_array_i64(_s64* arr, _s64 n)
; 길이 n인 배열 arr의 모든 원소의 합을 반환한다.

; 반복문을 사용하여 배열을 순회하고 합을 rax에 저장
sum_array_i64 proc
    ; 설명:
    ; rcx = arr 시작 주소, rdx = n
    ; TODO:
    ; 반복문을 사용하여 배열을 순회하고 합을 rax에 저장

    mov rax,    0
    mov r8,     0

_L:
    cmp r8,     rdx
    jge _E
    add rax,    qword ptr[rcx + 8 * r8]
    inc r8
    jmp _L

_E:
    ret
sum_array_i64 endp


; 9. max_array_i64(_s64* arr, _s64 n)
; 길이 n인 배열에서 최댓값을 반환한다.
max_array_i64 proc
    ; TODO:
    ; 첫 번째 값을 기준으로 잡고, 순회하면서 더 큰 값으로 갱신

    mov     rax,        0
    cmp     rdx,        0
    jle     _E

    mov     rax,        qword ptr[rcx]
    mov     r8,         1
_L:
    cmp     r8,         rdx
    jge     _E
    
    mov     r9,         qword ptr[rcx + r8 * 8]
    cmp     rax,        r9
    cmovl   rax,        r9
    inc     r8
    jmp     _L

_E:
    ret    
max_array_i64 endp


; 10. linear_search_i64(_s64* arr, _s64 n, _s64 value)
; 배열에서 value를 찾아 인덱스를 반환하고, 없으면 -1 반환.
linear_search_i64 proc
    ; 설명:
    ; rcx = arr, rdx = n, r8 = value
    ; TODO:
    ; 0..n-1까지 순회하며 arr[i] == value면 i 반환, 없으면 -1

    mov     r9,     0
    mov     rax,    -1
    cmp     rdx,    0
    jle     _E

_L: 
    cmp     r9,     rdx
    jge     _E
    mov     r10,    qword ptr[rcx + 8 * r9]
    cmp     r10,    r8
    cmove   rax,    r9
    je      _E
    inc     r9
    jmp     _L

_E:
    ret


linear_search_i64 endp


; 11. dot_product_i64(_s64* a, _s64* b, _s64 n)
; 두 배열 a, b의 내적 (a[i] * b[i])의 합을 반환한다.
dot_product_i64 proc
    ; 설명:
    ; rcx = a, rdx = b, r8 = n
    ; TODO:
    ; 반복문으로 a[i]*b[i]를 누적하여 rax에 저장

    push    rbx ; callee saved

    mov     rax,    0       ; sum
    mov     rbx,    0       ; i
    cmp     r8,     0
    jle     _E

_L:
    cmp     rbx,    r8
    jge     _E

    mov     r9,     qword ptr[rcx + rbx * 8]
    mov     r10,    qword ptr[rdx + rbx * 8]
    imul    r9,     r10
    add     rax,    r9
    inc     rbx
    jmp     _L

_E:
    pop     rbx
    ret
dot_product_i64 endp


; 12. strlen_ascii(char* s)
; 널 종료 ASCII 문자열 s의 길이를 반환한다(널 문자 제외).
strlen_ascii proc
    ; 설명: rcx = s
    ; TODO:
    ; 널 문자(0)를 만날 때까지 순회

    mov     rax,            rcx
    cmp     rcx,            0
    je      _E

L:
    cmp     byte ptr[rcx],  0
    je      _E
    inc     rcx
    jmp     L

_E: sub     rcx,            rax
    mov     rax,            rcx
    ret
    
strlen_ascii endp


; 13. strcmp_ascii(char* a, char* b)
; C의 strcmp처럼 동작: a < b 음수, a == b 0, a > b 양수 반환.
strcmp_ascii proc
    ; 설명: rcx = a, rdx = b
    ; TODO:
    ; 문자열을 비교하다가 다른 문자가 나오면 (a[i] - b[i]) 반환

    push    rbx
    mov     rax,    0
    cmp     rcx,    0
    je      _EXIT_A

    mov     rax,    1
    cmp     rdx,    0
    je      _E
    jmp     _START

_EXIT_A:
    cmp     rdx,    0
    jz      _E
    mov     rax,    -1
    pop     rbx
    ret

_START:
    movsx   rax,    byte ptr[rcx]
    movsx   rbx,    byte ptr[rdx]

    mov     r8,     rax
    sub     rax,    rbx
    jne     _E

    test    r8,     r8
    jz      _E

    inc     rcx
    inc     rdx

    jmp     _START

_E: pop     rbx
    ret

strcmp_ascii endp


; 14. to_upper_ascii(char* s)
; 문자열을 대문자로 변환 ('a'~'z' -> 'A'~'Z').
to_upper_ascii proc
    ; 설명: rcx = s
    ; TODO:
    ; 소문자 범위이면 32를 빼서 대문자로 변환

    test    rcx,    rcx
    jz      _E

_L:
    movsx   rax,    byte ptr[rcx]

    test    rax,    rax ; null break
    je      _E

    cmp     rax,    97
    jl      _S

    cmp     rax,    122
    jg      _S

    sub     rax,    32
    mov     byte ptr[rcx],  al

_S:
    inc     rcx
    jmp     _L
        
_E: 
    ret
to_upper_ascii endp


; 15. memcpy_u8(void* dst, void* src, _u64 n)
; src에서 dst로 n바이트를 복사한다(겹치지 않는 경우).
memcpy_u8 proc
    ; 설명: rcx = dst, rdx = src, r8 = n
    ; TODO:
    ; 바이트 단위로 복사하고, rax에 dst 반환

    mov     rax,    rcx
    test    r8,     r8
    jz      _E

_L:
    mov     r9b,     byte ptr[rdx]
    mov     byte ptr[rcx],  r9b

    dec     r8
    jz      _E

    inc     rdx
    inc     rcx
    jmp     _L

_E:
    ret


memcpy_u8 endp


end
