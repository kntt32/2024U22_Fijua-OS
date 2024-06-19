#include <types.h>
#include <kernel.h>
#include "functions.h"
#include "memory.h"
#include "console.h"

static uintn   Memory_PageCount = 0;
static uint16* Memory_MemMap = NULL;

extern KernelInputStruct* KernelInput;

void Memory_Init(void) {
    Memory_PageCount = KernelInput->Memory.PageCount;
    Memory_MemMap = (uint16*)(KernelInput->Memory.AvailableMemoryMap);
}


void Memory_Print_Memmap(void) {
    ascii strbuff[17];
    for(uintn i=0; i<Memory_PageCount; i++) {
        if(Memory_MemMap[i] != 0) {
            sprintintx(i, sizeof(strbuff), strbuff);
            Console_Print(strbuff);
            Console_Print("\n");
        }
    }
    return;
}


void* Memory_AllocatePages(uint16 ownerid, uintn pages) {
    /*
    ownerid:
        0:firmware
        1:available
        2:kernel
        10~:app
    */
    if(ownerid == 0) return NULL;
    if(pages == 0) return NULL;

    uintn i=0;
    uintn memareaPages = 0;
    for(uintn i=0; i<Memory_PageCount; i++) {
        if(Memory_MemMap[i] == 1) memareaPages++;
        if(memareaPages == pages) {
            for(int k=i-pages+1; k<=i; k++) {
                Memory_MemMap[k] = ownerid;
            }
            return (void*)((i-pages+1)<<12);
        }
    }

    return NULL;
}

uintn Memory_FreeAll(uint16 ownerid) {

}

uintn Memory_FreePages() {
    return 1;
}


