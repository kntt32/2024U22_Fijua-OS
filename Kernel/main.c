#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "graphic.h"
#include "console.h"
#include "timer.h"
#include "memory.h"
#include "queue.h"
#include "task.h"
#include "layer.h"
#include "mouse.h"
#include "syscall.h"

#include "terminal.h"

#include "test.h"

KernelInputStruct* KernelInput = NULL;

int kernel_main(KernelInputStruct* kernelInput) {
    KernelInput = kernelInput;

    Graphic_Init();
    Console_Init();
    
    Memory_Init();
    Timer_Init();
    Mouse_Init();

    Task_Init();
    Layer_Init();

    Syscall_Init();

    Task_New(terminal_main);
/*
    Layer_Window_New(2, "testWindow", 10, 10, 300, 400);

    Layer_Window_New(2, "test2", 50, 50, 100, 60);
*/
    HltLoop();

    return 1;
}
