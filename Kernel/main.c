#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "font.h"
#include "console.h"
#include "timer.h"
#include "memory.h"

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

    uint32* framebuffptr = (uint32*)(kernelInput->Graphic.startAddr);
    for(int i=0; i<100; i++) {
        *framebuffptr = 0xFFFFFFFF;
        framebuffptr++;
    }

    Font_Init();
    Console_Init();
    Memory_Init();
    Timer_Init();

    wrapper((void*)(kernelInput->LoadedImage->SystemTable->ConOut->OutputString), (uintn)(kernelInput->LoadedImage->SystemTable->ConOut), (uintn)(L"HELLO!"), 0, 0, 0);


    Console_Print("Hello, Console!\n");
    Console_Print("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=^~\\|[]{}@:;*+./?<>!_#\"$%&\'()\n");

    ascii strbuff[17];
    void* memptr1 =  Memory_AllocPages(2, 2);
    Functions_SPrintIntX((uintn)memptr1, 17, strbuff);
    Console_Print(strbuff);
    Console_Print("\n");
    void* memptr2 =  Memory_AllocPages(2, 2);
    Functions_SPrintIntX((uintn)memptr2, 17, strbuff);
    Console_Print(strbuff);
    Console_Print("\n");
    Memory_FreePages(2, 1, memptr1+0x1000);
    void* memptr3 = Memory_AllocPages(2, 1);
    Functions_SPrintIntX((uintn)memptr3, 17, strbuff);
    Console_Print(strbuff);
    Console_Print("\n");
    
    Memory_FreeAll(2);


    Halt();

    return 1;
}
