.data



.text

.global Efi_Wrapper
.global Timer_Wrapper
.global Mutex_Lock
.global Mutex_UnLock
.global Task_ContextSwitch
.global Task_NewTask_Asm_SetStartContext
.global Task_WaitTaskSwitch


# uintn Efi_Wrapper(uintn (*callback)(), ...);
Efi_Wrapper:
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


# void Timer_Wrapper(void*, uintn (*callback)(void));
Timer_Wrapper:
    push %rbp
    push %rdi
    push %rsi

    call *%rdx

    pop %rsi
    pop %rdi
    pop %rbp

    ret


# void Mutex_Lock(uintn* lockvar);
Mutex_Lock:
    cmpq $0, (%rdi)
    je Mutex_Lock_GetLock
    pause
    jmp Mutex_Lock
Mutex_Lock_GetLock:
    movq $1, %rax
    xchg (%rdi), %rax
    cmpq $0, %rax
    jne Mutex_Lock
    ret


# void Mutex_UnLock(uintn* lockvar);
Mutex_UnLock:
    movq $0, %rax
    xchg %rax, (%rdi)
    cmpq $1, %rax
    jne Mutex_UnLock_Err
    movq $0, %rax
    ret
Mutex_UnLock_Err:
    movq $1, %rax
    ret


# void Task_ContextSwitch(void);
Task_ContextSwitch:
    push %rax
    push %rbp
    push %r12
    push %r13
    push %r14
    push %r15
    push %rbx

    mov %rsp, %rdi
    call Task_ContextSwitch_Subroutine
    mov %rax, %rsp

    pop %rbx
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbp
    pop %rax

    ret


# void* Task_NewTask_Asm_SetStartContext(void* stackptr);
# input: stack ptr
# return: context saved stack ptr
Task_NewTask_Asm_SetStartContext:
    and $0xfffffffffffffff0, %rdi
    mov %rsp, %rax
    mov %rdi, %rsp
    
    leaq Task_NewTask_StartPoint(%rip), %r10

    pushq $0x0 #dummy

    pushq %r10   #rip

    pushq $0x0                      #rax
    pushq $0x0                  #rbp
    pushq $0x0                  #r12
    pushq $0x0                  #r13
    pushq $0x0                  #r14
    pushq $0x0                  #r15
    pushq $0x0                  #rbx

    mov %rsp, %rdi
    mov %rax, %rsp

    mov %rdi, %rax
    ret


# void Task_WaitTaskSwitch(uintn* switchCount);
Task_WaitTaskSwitch:
    mov (%rdi), %r10
Task_WaitTaskSwitch_Loop:
    mov (%rdi), %r11
    cmp %r10, %r11
    je Task_WaitTaskSwitch_Loop

    ret

