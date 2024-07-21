#include <types.h>
#include <efi.h>
#include <kernel.h>
#include "functions.h"
#include "x64.h"
#include "queue.h"
#include "task.h"
#include "console.h"

extern KernelInputStruct* KernelInput;


/* Miscellaneous Functions */

//Halt (BootServices->Stall)
void Halt() {
    while(1) {
        Task_Yield();
        Halt_Asm_Hlt();
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


//MemCpy
void Functions_MemCpy(void* to, void* from, uintn size) {
    uint64* targTo64 = (uint64*)to;
    uint64* targFrom64 = (uint64*)from;
    for(uintn i=0; i<(size >> 3); i++) {
        *targTo64 = *targFrom64;
        targTo64++;
        targFrom64++;
    }
    uint8* targTo8 = (uint8*)targTo64;
    uint8* targFrom8 = (uint8*)targFrom64;
    for(uintn i=0; i<(size&0x07); i++) {
        *targTo8 = *targFrom8;
        targTo8++;
        targFrom8++;
    }
    return;
}
