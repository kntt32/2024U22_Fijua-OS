#include <types.h>
#include <kernel.h>
#include "syscall.h"
#include "queue.h"
#include "task.h"
#include "x64.h"
#include "console.h"
#include "graphic.h"
#include "layer.h"

#define Syscall_SyscallAddr ((void**)0x100000)

uintn Syscall_AppEnter();

void Syscall_Init(void) {
    *Syscall_SyscallAddr = (void*)Syscall_AppEnter;

    return;
}

//新規ウインドウ作成してレイヤIDを返す
sintn Syscall_NewWindow(in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]) {
    Task_Yield();

    uint16 taskId = Task_GetRunningTaskId();

    uintn layerId = Layer_Window_New(taskId, title, x, y, width, height);

    return layerId;
}


//CPUを解放
sintn Syscall_YieldCpu(void) {
    Task_Yield();
    return 0;
}
