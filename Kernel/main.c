#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "font.h"
#include "console.h"
#include "timer.h"

/*
https://docs.oracle.com/cd/E19253-01/819-0389/fcowb/index.html
https://ameblo.jp/reverse-eg-mal-memo/entry-12608477931.html
https://ja.wikipedia.org/wiki/%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84#%E3%83%9E%E3%82%A4%E3%82%AF%E3%83%AD%E3%82%BD%E3%83%95%E3%83%88_x64%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84
*/

KernelInputStruct* KernelInput = NULL;

void a() {
    Font_Draw("A", 0,32,0xff,0xff,0xff);
}

int kernel_main(KernelInputStruct* kernelInput) {
    KernelInput = kernelInput;

    uint32* framebuffptr = (uint32*)(kernelInput->Graphic.startAddr);
    for(int i=0; i<100; i++) {
        *framebuffptr = 0xFFFFFFFF;
        framebuffptr++;
    }

    Font_Init();
    Font_Draw_WhiteFont('A', 0, 0);
    Font_Draw("Hello, World!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 0, 16, 0xff, 0xff, 0xff);

    wrapper((void*)(kernelInput->LoadedImage->SystemTable->ConOut->OutputString), (uintn)(kernelInput->LoadedImage->SystemTable->ConOut), (uintn)(L"HELLO!"), 0, 0, 0);
    a();

    Console_Init();
    Console_Print("Hello, Console!\n");
    Console_Print("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=^~\\|[]{}@:;*+./?<>!_#\"$%&\'()");

    Timer_Init();
    Timer_Set(NULL, 10000000);

    Console_Print("A\n");

    Halt();

    return 1;
}
