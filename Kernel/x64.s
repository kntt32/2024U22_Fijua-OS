.data



.text

.global Efi_Wrapper
.global Timer_Wrapper
.global Mutex_Lock
.global Mutex_UnLock
.global Task_ContextSwitch
.global Task_NewTask_Asm


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
    push %rsp
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
    pop %rsp

    ret


# void Task_WaitSwitch(uintn* switchCount, uint8* enableChangeTaskFlag)
Task_WaitSwitch:
    movb $0x1, (%rsi)
    movq (%rdi), %r10
Task_WaitSwitch_Loop:
    pause
    movq (%rdi), %rsi
    cmp %rsi, %r10
    je Task_WaitSwitch_Loop

    movb $0x0, (%rsi)
    
    ret



# uint16 Task_NewTask_Asm(uintn* switchCount, uint8* enableChangeTaskFlag, sintn (*entry)(void));
Task_NewTask_Asm:
    push %rbp
    mov %rsp, %rbp

    push %rdi   #uintn* switchCount;
    push %rsi   #uint8* enableChangeTaskFlag;
    push %rdx   #sintn (*entry)(void);

    pushq $0x0  #uintn taskid = 0;

#wait for taskswitch

    movb $0x1, -0x10(%rbp)   #*switchCount = 1; enable taskswitch
    mov -0x8(%rbp), %r10     #save switchCount

Task_NewTask_Asm_Loop:
    mov -0x8(%rbp), %r11
    mov (%r11), %r11
    cmp %r10, %r11
    je Task_NewTask_Asm_Loop

    movb $0x0, -0x10(%rbp)   #disable taskswitch

    cmpq $0x0, -0x20(%rbp)
    jne Task_NewTask_Ret    #if(flag) goto Task_NewTask_Ret

#jump to newtask
    leaq -0x20(%rbp), %rdi
    call Task_NewTask_Asm_AddTaskTable
    cmpq $0x0, -0x20(%rbp)
    je Task_NewTask_Ret

    andq $0xfffffffffffffff0, %rax
    mov %rax, %rsp
    pushq $0x00
    mov -0x18(%rbp), %rax
    movb $0x1, -0x10(%rbp)   #enable taskswitch
    jmpq *%rax

Task_NewTask_Ret:
    movb $0x1, -0x10(%rbp)   #enable taskswitch

    mov -0x20(%rbp), %rax

    mov %rbp, %rsp
    pop %rbp

    ret

