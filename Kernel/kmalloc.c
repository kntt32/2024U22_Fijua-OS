/*
Boundary Tags
struct {
    uint64 memAreaSize;
    uint64 flag;//0:available 1:using
    uint64 memArea[memAreaSize];
    uint64 memAreaSize_;
}
*/

#include <types.h>
#include <kernel.h>
#include "x64.h"
#include "functions.h"
#include "kmalloc.h"
#include "console.h"
#include "memory.h"

static uintn KMalloc_PagesCount = 0;
static uintn KMalloc_PagesPoolSize = 0;
static uint64** KMalloc_PagesPool = NULL;
static uintn*  Kmalloc_PagesPool_EachPagesCounts = NULL;

static uintn KMalloc_Mutex = 0;


static sintn KMalloc_AddPagePool(uintn pages) {
    if(pages == 0) return 1;

    //allocate memory for pagepool list
    if(KMalloc_PagesPoolSize == KMalloc_PagesCount) {
        uint64** KMalloc_PagesPool_Temp = Memory_AllocPages(2, (((KMalloc_PagesPoolSize==0)?(1):(KMalloc_PagesPoolSize))*2*sizeof(uint64*)+0xfff)>>12);
        uintn*   Kmalloc_PagesPool_EachPagesCounts_Temp = Memory_AllocPages(2, (((KMalloc_PagesPoolSize==0)?(1):(KMalloc_PagesPoolSize))*2*sizeof(uintn)+0xfff)>>12);
        if(KMalloc_PagesPool_Temp == NULL) return -1;
        //for()//copy kmalloc_pagepool
        //free old kmalloc_pagepool
        //update kmalloc_pagespoolsize
    }



    return 0;
}


static void KMalloc_AllocArea(uint64* targTag, uintn size) {
    if(targTag[0]-size <= 2) {
        size=targTag[0];
    }
    targTag[targTag[0]+2] = targTag[0]-3-size;
    targTag[targTag[0]+2 - (targTag[0]-3-size)-1] = 0;//targTag[3+size+1] = 0;
    targTag[targTag[0]+2 - (targTag[0]-3-size)-2] = targTag[0]-3-size;//targTag[3+size] = targTag[0]-3-size;
    targTag[2+size] = size;
    targTag[0] = size;
    targTag[1] = 1;
    return;
}


void* KMalloc_Allocate(uintn size) {
    if(size == 0) return NULL;

    Mutex_Lock(&KMalloc_Mutex);

    //seek pagepool
    for(uintn i=0; i<KMalloc_PagesCount; i++) {
        uint64* targTag = KMalloc_PagesPool[i];
        uintn k=0;
        while(k<512*Kmalloc_PagesPool_EachPagesCounts[i]) {
            if(targTag[1]==0 && size<=targTag[0]) {
                KMalloc_AllocArea(targTag, size);
                return targTag+2;
            }else {
                targTag += targTag[0]+3;
            }
        }
    }

    //KMalloc_AddPagePool();

    Mutex_UnLock(&KMalloc_Mutex);

    return NULL;
}

