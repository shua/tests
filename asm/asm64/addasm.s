section .text
	global asmadd
asmadd:
	push rbp
	mov rbp, rsp
	mov [rbp-8], rdi
	mov [rbp-16], rsi
	mov rsi, [rbp-8]
	add rsi, [rbp-16]
	mov rax, rsi
	pop rbp
	ret

