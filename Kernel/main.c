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
    Console_Print("Initializing Mouse...\n");
#if 1
    Mouse_Init();
    Console_Print("Mouse Initialization Done!\n");

    Task_Init();
    Layer_Init();
    
    while(1) {
        Layer_Update();
    }

    Halt();

    Task_NewTask(Test_Tester1);
    Task_NewTask(Test_Tester2);
    Task_Yield();
#endif

    Console_Print("Hello!\n");
    Console_Print("iadgilfuhsoiuhaoisughoiasuhgioauhgoiuashogiuhaoiruhgoisauhgroiaurhgoisuahgiouhsarg");
    ascii strbuff[18];
    for(uintn i=0; i<100; i++) {
        SPrintIntX(i, 17, strbuff);
        strbuff[16] = '\n';
        strbuff[17] = '\0';
        Console_Print(strbuff);
    }

    Halt();

    return 1;
}
