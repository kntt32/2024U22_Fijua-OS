#ifndef INCLUDED_X64_H
#define INCLUDED_X64_H

uintn wrapper(void* callback, uintn, uintn, uintn, uintn, uintn);

void Timer_Wrapper(void*, uintn (*callback)(void));

void Mutex_Lock(uintn* lockvar);

void Mutex_UnLock(uintn* lockvar);


#endif
