#ifndef INCLUDED_X64_H
#define INCLUDED_X64_H

uintn Efi_Wrapper(void* callback, ...);

void Mutex_Lock(uintn* lockvar);

void Mutex_UnLock(uintn* lockvar);


#endif
