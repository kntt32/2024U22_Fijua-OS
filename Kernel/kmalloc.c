/*
    Memory Allocater

    note: this allocater using boundary tag algolithm
*/

#include <types.h>
#include <kernel.h>
#include "functions.h"
#include "memory.h"
#include "console.h"
#include "kmalloc.h"


#define KMALLOC_MAX_MANAGE_PAGE_COUNT (10000)


extern KernelInputStruct* KernelInput;


static uintn KMalloc_ManagedPageCount = 0;
static uintn KMalloc_ManagedPageList[KMALLOC_MAX_MANAGE_PAGE_COUNT];


uintn KMalloc_Init(void) {
    KMalloc_ManagedPageCount = 0;
    return 0;
}


void* KMalloc_Alloc(uintn size) {
    if(size == 0) return NULL;



    return NULL;
}


void* KMalloc_Realloc() {

}


void KMalloc_Free() {

}

