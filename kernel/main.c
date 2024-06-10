#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "font.h"

/*
https://docs.oracle.com/cd/E19253-01/819-0389/fcowb/index.html
https://ameblo.jp/reverse-eg-mal-memo/entry-12608477931.html
https://ja.wikipedia.org/wiki/%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84#%E3%83%9E%E3%82%A4%E3%82%AF%E3%83%AD%E3%82%BD%E3%83%95%E3%83%88_x64%E5%91%BC%E5%87%BA%E8%A6%8F%E7%B4%84
*/

#define NULL ((void*)0)

sintn wrapper(void*, void*, void*, void*, void*, void*);

void a() {
    Font_Draw("A", 0,32,0xff,0xff,0xff);
}

int kernel_main(KernelInputStruct* kernelInput) {
    uint32* framebuffptr = (uint32*)(kernelInput->Graphic.startAddr);
    for(int i=0; i<100; i++) {
        *framebuffptr = 0xFFFFFFFF;
        framebuffptr++;
    }

    Font_Init(kernelInput);
    Font_Draw_WhiteFont('A', 0, 0);
    Font_Draw("Hello, World!", 0, 16, 0xff, 0xff, 0xff);

    wrapper((void*)(kernelInput->LoadedImage->SystemTable->ConOut->OutputString), (void*)(kernelInput->LoadedImage->SystemTable->ConOut), (void*)(L"HELLO!"), NULL, NULL, NULL);
    a();
    return 1;
}
