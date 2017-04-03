
_hex: db "0123456789abcdef!"
_hex_out: db "0000", 0
_hex_ret: dw 0
_test: dw 0

PrintHex:
    mov [_hex_ret], bx
    xor bx, bx
    mov ds, bx

    mov bl, al
    shr bl, 4
    mov bh, [_hex+bx]
    mov [_hex_out+2], bh
    xor bx, bx
    mov bl, al
    and bl, 0fh
    mov bh, [_hex+bx]
    mov [_hex_out+3], bh
    xor bx, bx
    mov bl, ah
    shr bl, 4
    mov bh, [_hex+bx]
    mov [_hex_out+0], bh
    xor bx, bx
    mov bl, ah
    and bl, 0fh
    mov bh, [_hex+bx]
    mov [_hex_out+1], bh    
    
    mov dh, 18h
    mov dl, 4ch
    mov bx, 000fh
    mov cx, 1
    cld
    mov si, _hex_out
_hex_char:
    mov ah, 2
    int 10h
    lodsb

    mov ah, 09h
    int 10h

    inc dl
;    cmp dl, 80
;    jne _hex_skip
;    xor dl, dl

;    inc dh
;    cmp dh, 60
;    jne _hex_skip
;    xor dh, dh
_hex_skip:
    mov ah, [si]
    cmp ah, 0
    jne _hex_char
_hex_end:
    mov bx, [_hex_ret]
    jmp bx

