#include <types.h>
#include <kernel.h>
#include "memory.h"
#include "console.h"

static uintn   Memory_MemSize = 0;
static uint16* Memory_MemMap = NULL;

extern KernelInputStruct* KernelInput;

uintn Memory_Init() {
    Memory_MemSize = KernelInput->Ram->RamSize;
    Memory_MemMap = KernelInput->Ram->AvailableRamMap;
}


uintn Memory_Print_Memmap() {

}


uintn Memory_AllocatePages() {

}


uintn Memory_FreePages() {

}


