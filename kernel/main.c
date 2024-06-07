#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "font.h"

int kernel_main(KernelInputStruct* kernelInput) {
    uint32* framebuffptr = (uint32*)(kernelInput->Graphic.startAddr);
    for(int i=0; i<100; i++) {
        *framebuffptr = 0xFFFFFFFF;
        framebuffptr++;
    }

    Font_Init(kernelInput);
    Font_Draw_WhiteFont('A', 0, 0);
    Font_Draw("Hello, World!", 0, 16, 0xff, 0xff, 0xff);

    return 1;
}
