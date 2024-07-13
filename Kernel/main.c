#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "graphic.h"
#include "font.h"
#include "console.h"
#include "timer.h"
#include "memory.h"
#include "queue.h"
#include "task.h"

/*
https://docs.oracle.com/cd/E19253-01/819-0389/fcowb/index.html
https://ameblo.jp/reverse-eg-mal-memo/entry-12608477931.html
https://ja.wikipedia.org/wiki/%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84#%E3%83%9E%E3%82%A4%E3%82%AF%E3%83%AD%E3%82%BD%E3%83%95%E3%83%88_x64%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84
*/

KernelInputStruct* KernelInput = NULL;

void a() {
    static int n=0;
    switch(n) {
    case 0:
        Console_Print("A\n");
        break;
    case 1:
        Console_Print("B\n");
        break;
    case 2:
        Console_Print("C\n");
        break;
    }
    n++;
    n = n%3;
}

int kernel_main(KernelInputStruct* kernelInput) {
    KernelInput = kernelInput;

    Graphic_Init();
    Console_Init();
    
    Memory_Init();
    Timer_Init();

    //Task_Init();




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
