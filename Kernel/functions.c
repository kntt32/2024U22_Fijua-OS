#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "console.h"

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


void printk(ascii* str) {

}


void sprintintx(uintn number, uintn buffsize, ascii buff[]) {
    uintn n;
    for(sintn i=buffsize-2; 0<=i; i--) {
        n = number & 0xf;
        buff[i] = (n<10)?(n+'0'):(n+'a'-10);
        
        number >>= 4;
    }
    buff[buffsize-1] = '\0';
    
    return;
}





