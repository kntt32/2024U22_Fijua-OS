#ifndef INCLUDED_ELFLOADER_H
#define INCLUDED_ELFLOADER_H

#include <types.h>

typedef struct {
    void* startAddr;
    uintn memSize;
} ElfLoader_MemLoadArea;

unsigned int ElfLoader_GetLoadArea(const void* file, ElfLoader_MemLoadArea* buff, uintn* buffCount);

#endif
