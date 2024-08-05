#ifndef INCLUDED_FUNCTIONS_H
#define INCLUDED_FUNCTIONS_H

void HltLoop(void);

void SPrintIntX(uintn number, uintn buffsize, ascii buff[]);

sintn Log2(uintn number);

void Functions_MemCpy(void* to, const void* from, uintn size);

uintn Functions_UTF16LE2ASCII(uint16 input, ascii* output);

#endif
