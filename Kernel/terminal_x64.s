.global Terminal_Syscall_NewWindow
.global Terminal_Syscall_YieldCpu
.global Terminal_Syscall_DrawSquare
.global Terminal_Syscall_DrawFont
.global Terminal_Syscall_ReadMessage

.text

Terminal_Syscall_NewWindow:
    mov $0, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_YieldCpu:
    mov $1, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_DrawSquare:
    mov $2, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_DrawFont:
    mov $3, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_ReadMessage:
    mov $4, %rax
    callq *(0x100000)
    ret
