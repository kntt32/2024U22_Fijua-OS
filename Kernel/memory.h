#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H


void Memory_Init(void);

void Memory_Print_Memmap(void);

void* Memory_AllocatePages(uint16 ownerid, uintn pages);

#endif
