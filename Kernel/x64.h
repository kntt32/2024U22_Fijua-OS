#ifndef INCLUDED_X64_H
#define INCLUDED_X64_H

uintn Efi_Wrapper(void* callback, ...);

void Timer_Wrapper(void*, uintn (*callback)(void));

void Mutex_Lock(uintn* lockvar);

void Mutex_UnLock(uintn* lockvar);

void Task_ContextSwitch(void** saveRspTo, void* switchRspTo);


#endif
