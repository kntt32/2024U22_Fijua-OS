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


void Functions_SPrintIntX(uintn number, uintn buffsize, ascii buff[]) {
    uintn n;
    for(sintn i=buffsize-2; 0<=i; i--) {
        n = number & 0xf;
        buff[i] = (n<10)?(n+'0'):(n+'a'-10);
        
        number >>= 4;
    }
    buff[buffsize-1] = '\0';
    
    return;
}


sintn Functions_Log2(uintn number) {
    for(sintn i=0; i<64; i++) {
        if(number>>i == 0) return i-1;
    }
    return -1;
}

/*
void Functions_MemDump(void* start, uintn count16) {
    ascii buff[]
    for(uintn i=0; i<count16; i++) {

    }

    return;
}*/


void BlueScreen(uintn errCode, ascii moduleName[]) {


    Halt();
}


