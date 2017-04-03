    org 7c00h
    jmp short Start

cafebabe: dw 0

Start:
    mov ax, [cafebabe]
    cmp ax, 0
    jne _babe
    inc ax
    mov [cafebabe], ax
    mov ax, 0cafeh
    jmp _print
_babe:
    xor ax, ax
    mov [cafebabe], ax
    mov ax, 0babeh
_print:
    mov bx, Busy
    jmp PrintHex
Busy:
    xor ax, ax
    xor bx, bx
_Busy:
    inc ax
    cmp ax, 0ffffh
    jne _Busy
    xor ax, ax
    inc bx
    cmp bx, 0fffh
    jne _Busy
    jmp Start

%include "phex.s"
times 0200h - 2 - ($- $$) db 0
    dw 0aa55h
