.global wrapper_callKernel

wrapper_callKernel:
	# rcx: kernelInput
	
	push %rbx
	push %rsi
	push %rdi

	mov %rcx, %rdi
	mov entryPoint, %rax
	callq *%rax

	pop %rdi
	pop %rsi
	pop %rbx

	ret
