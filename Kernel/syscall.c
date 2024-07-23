#include <types.h>
#include <kernel.h>
#include "syscall.h"
#include "queue.h"
#include "task.h"
#include "x64.h"
#include "console.h"

#define Syscall_SyscallAddr ((void**)0x100000)

uintn Syscall_AppEnter();

void Syscall_Init(void) {
    *Syscall_SyscallAddr = (void*)Syscall_AppEnter;

    return;
}
