#include <stdio.h>
#include <stdlib.h>

#include "elfloader.h"

int main() {
    FILE* fptr = fopen("a.out", "rb");
    if(fptr == NULL) return 1;

    char* buff = malloc(1000);
    fread(buff, 1, 1000, fptr);

    for(unsigned int i=0; i<100; i++) {
        printf("%x,\n", buff[i]);
    }

    ElfLoader_MemLoadArea* mlaptr = malloc(sizeof(ElfLoader_GetLoadArea)*50);

    uintn mlacount = 50;

    printf("%d\n",ElfLoader_GetLoadArea(buff, mlaptr, &mlacount));
    printf("%d\n", mlacount);
    for(unsigned int i=0; i<mlacount; i++) {
        printf("%p: %x\n", mlaptr[i].startAddr, mlaptr[i].memSize);
    }


    fclose(fptr);

    return 0;
}
