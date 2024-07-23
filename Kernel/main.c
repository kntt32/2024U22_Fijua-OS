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



    Task_Yield();

    Layer_Window_New(2, "testWindow", 10, 10, 300, 400);

    Layer_Window_New(2, "test2", 50, 50, 100, 60);

    Console_Print("B");

    Task_Yield();

    Task_New(terminal_main);

    Halt();

    Console_Print("hAllo");

    Task_New(Test_Tester1);
    Task_New(Test_Tester2);
    Task_Yield();

    Console_Print("Hello!\n");
    Console_Print("iadgilfuhsoiuhaoisughoiasuhgioauhgoiuashogiuhaoiruhgoisauhgroiaurhgoisuahgiouhsarg");
    ascii strbuff[18];
    for(uintn i=0; i<100; i++) {
        SPrintIntX(i, 17, strbuff);
        strbuff[16] = '\n';
        strbuff[17] = '\0';
        Console_Print(strbuff);
        Layer_Update();
    }

    Halt();

    return 1;
}
