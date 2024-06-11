.global entrypoint

entrypoint:
    # rcx: kernelInput
    push %rbx
    push %rsi
    push %rdi

    mov %rcx, %rdi
    call kernel_main

    pop %rdi
    pop %rsi
    pop %rbx

    ret