#ifndef INCLUDED_X64_H
#define INCLUDED_X64_H

uintn wrapper(void* callback, uintn, uintn, uintn, uintn, uintn);

void Timer_Wrapper(uintn (*callback)(void));

#endif
