#include <types.h>
#include <kernel.h>
#include "functions.h"
#include "memory.h"
#include "console.h"
#include "memallocater.h"


#define KMALLOC_MAX_MANAGE_PAGE_COUNT (10000)


extern KernelInputStruct* KernelInput;


static uintn KMalloc_ManagedPageCount = 0;
static uintn KMalloc_ManagedPageList[KMALLOC_MAX_MANAGE_PAGE_COUNT];


uintn KMalloc_Init(void) {
    KMalloc_ManagedPageCount = 0;
    return 0;
}


uintn KMalloc_Alloc() {

}


uintn KMalloc_Realloc() {

}


void KMalloc_Free() {

}

