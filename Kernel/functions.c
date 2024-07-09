#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "console.h"

extern KernelInputStruct* KernelInput;


/* Miscellaneous Functions */

//Halt (BootServices->Stall)
void Halt() {
    static EFI_STALL Stall = NULL;

    if(Stall == NULL) {
        Stall = KernelInput->LoadedImage->SystemTable->BootServices->Stall;
    }
    
    while(1) {
        Efi_Wrapper((void*)Stall, 1, 0, 0 ,0 ,0);
    }
}


//convert number to string
void SPrintIntX(uintn number, uintn buffsize, ascii buff[]) {
    uintn n;
    for(sintn i=buffsize-2; 0<=i; i--) {
        n = number & 0xf;
        buff[i] = (n<10)?(n+'0'):(n+'a'-10);
        
        number >>= 4;
    }
    buff[buffsize-1] = '\0';
    
    return;
}


//log2
sintn Log2(uintn number) {
    for(sintn i=0; i<64; i++) {
        if(number>>i == 0) return i-1;
    }
    return -1;
}


//blueScreen
void BlueScreen(ascii moduleName[], ascii errStr[]) {


    Halt();
}

