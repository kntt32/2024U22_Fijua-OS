#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"

extern KernelInputStruct* KernelInput;


void Halt() {
    static EFI_STALL Stall = NULL;

    if(Stall == NULL) {
        Stall = KernelInput->LoadedImage->SystemTable->BootServices->Stall;
    }
    
    while(1) {
        wrapper((void*)Stall, 0xffffffff, 0, 0 ,0 ,0);
    }
}


