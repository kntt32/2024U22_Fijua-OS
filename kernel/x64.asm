.global wrapper

#uintn wrapper(uintn (*f)(), uintn input1, uintn input2, uintn input3, uintn input4, uintn input5);
wrapper:
    push %r9
    push %r8
    push %rcx
    push %rdx
    push %rsi

    mov %r8, %r9
    mov %rcx, %r8
    mov %rdx, %rdx
    mov %rsi, %rcx

    call *%rdi

    pop %r9
    pop %r9
    pop %r9
    pop %r9
    pop %r9

    ret
