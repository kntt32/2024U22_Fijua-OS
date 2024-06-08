.global wrapper

wrapper:
	push %rax
	push %r9
	push %r8

	mov %rdi, %rcx
	mov %rsi, %rdx
	mov %rdx, %r8
	mov %rcx, %r9

	dec $4, %rsp

	call *%r10

	a:
	jmp a

	ret
	