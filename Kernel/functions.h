#ifndef INCLUDED_FUNCTIONS_H
#define INCLUDED_FUNCTIONS_H

void Halt();

void SPrintIntX(uintn number, uintn buffsize, ascii buff[]);

sintn Log2(uintn number);

void Functions_MemCpy(void* to, void* from, uintn size);

#endif
