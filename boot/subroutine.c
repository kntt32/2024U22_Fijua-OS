#include <types.h>
#include <efi.h>
#include <efi_types.h>

#include "subroutine.h"


int Functions_SPrintIntX(uintn number, uintn buffsize, CHAR16 buff[]) {
    uintn n = 0;
    for(sintn i=buffsize-2; 0<=i; i--) {
        n = number & 0xf;
        buff[i] = (n<10)?(n+L'0'):(n+L'a'-10);
        number >>= 4;
    }
    buff[buffsize-1] = L'\0';
    
    return 0;
}

