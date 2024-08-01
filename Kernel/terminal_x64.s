.global Terminal_Syscall_NewWindow
.global Terminal_Syscall_CloseWindow
.global Terminal_Syscall_YieldCpu
.global Terminal_Syscall_DrawSquare
.global Terminal_Syscall_DrawFont
.global Terminal_Syscall_ReadMessage
.global Terminal_Syscall_CheckMessage
.global Terminal_Syscall_SendITCMessage
.global Terminal_Syscall_Exit

.text

Terminal_Syscall_NewWindow:
    mov $0, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_CloseWindow:
    mov $1, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_YieldCpu:
    mov $2, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_DrawSquare:
    mov $3, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_DrawFont:
    mov $4, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_ReadMessage:
    mov $5, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_CheckMessage:
    mov $6, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_SendITCMessage:
    mov $7, %rax
    callq *(0x100000)
    ret

Terminal_Syscall_Exit:
    mov $8, %rax
    callq *(0x100000)
    ret
