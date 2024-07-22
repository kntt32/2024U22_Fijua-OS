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

    Console_Print("A");

    Task_Yield();

    Layer_Window_New(2, "testWindow", 10, 10, 300, 400, 300, 400);

    Console_Print("B");

    Task_Yield();

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
