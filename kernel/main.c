#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "font.h"

int kernel_main(KernelInputStruct* kernelInput) {
    Font_Init(kernelInput);
    Font_Draw_WhiteFont('A', 0, 0);

    return 12;
}
