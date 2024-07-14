#ifndef INCLUDED_X64_H
#define INCLUDED_X64_H

uintn Efi_Wrapper(void* callback, ...);

void Timer_Wrapper(void*, uintn (*callback)(void));

void Mutex_Lock(uintn* lockvar);

void Mutex_UnLock(uintn* lockvar);

void Task_ContextSwitch(void);

uint16 Task_NewTask_Asm(uintn* switchCount, uint8* enableChangeTaskFlag, sintn (*entry)(void));


#endif
