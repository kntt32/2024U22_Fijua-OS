#include <types.h>
#include <kernel.h>
#include "functions.h"
#include "memory.h"
#include "console.h"

static uintn   Memory_MemSize = 0;
static uint16* Memory_MemMap = NULL;

extern KernelInputStruct* KernelInput;

void Memory_Init(void) {
    Memory_MemSize = KernelInput->Ram.RamSize;
    Memory_MemMap = (uint16*)(KernelInput->Ram.AvailableRamMap);
}


void Memory_Print_Memmap(void) {
    ascii strbuff[17];
    for(uintn i=0; i<Memory_MemSize; i++) {
        if(Memory_MemMap[i] != 0) {
            sprintintx(i, sizeof(strbuff), strbuff);
            Console_Print(strbuff);
            Console_Print("\n");
        }
    }
    return;
}


uintn Memory_AllocatePages() {
    return 1;
}


uintn Memory_FreePages() {
    return 1;
}


