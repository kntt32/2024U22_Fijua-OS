.global wrapper

#uintn wrapper(uintn input1, uintn input2, uintn input3, uintn input4, uintn input5, uintn (*f)());
wrapper:
	mov %r9, %rax

	push %r8
	push %rcx
	push %rdx
	push %rsi
	push %rdi

	mov %rcx, %r9
	mov %rdx, %r8
	mov %rsi, %rdx
	mov %rdi, %rcx

	call *%rax

	pop %r9
	pop %r9
	pop %r9
	pop %r9
	pop %r9

	ret
	