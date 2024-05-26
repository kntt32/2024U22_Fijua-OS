#include <stdio.h>
#include <stdlib.h>

#include "elfloader.h"

void put(char* str) {
    printf("%s", str);
}

char* strintxAsc(char* buff, unsigned int buffsize, unsigned int num) {
    if(buffsize < 3) {
        return NULL;
    }
    for(int i=0; i<buffsize-1; i++) {
        buff[i] = (num >> ((buffsize-i) << 2)) & 0xF;
        if(buff[i] > 9) {
            buff[i] += 'A' - 10;
        }else {
            buff[i] += '0';
        }
    }
    buff[buffsize-1] = '\0';
    return buff;
}

int ELFLoader_MemoryDump(void* ptr, unsigned int length) {
    char str[17];
    
    put("\n");
    for(int i=0; i<(length>>4)+1; i++) {
        strintxAsc(str, 17, i<<4);
        put(str);
        put(":");
        for(int k=0; k<16; k++) {
            if((k&0x3) == 0) {
                put(" ");
            }
            
            if((i<<4)+k == length) {
                put("\n");
                return 0;
            }
            strintxAsc(str, 3, *((unsigned char*)(ptr+(i << 4)+k)));
            str[2] = ' ';
            str[3] = '\0';
            put(str);
        }
        put("\n");
    }
    return 0;
}

int main() {
    FILE* fptr = fopen("a.out", "rb");
    if(fptr == NULL) return 1;

    char* buff = malloc(20000);
    fread(buff, 1, 20000, fptr);

    ElfLoader_MemLoadArea* mlaptr = malloc(sizeof(ElfLoader_GetLoadArea)*50);

    for(unsigned int i=0; i<100; i++) {
        for(unsigned int k=0; k<16; k++) {
            printf("%d ", mlaptr[i*16 + k]);
        }
        printf("\n");
    }

    uintn mlacount = 50;

    printf("%d\n",ElfLoader_GetLoadArea(buff, 0, &mlacount, mlaptr));
    printf("%d\n", mlacount);
    unsigned int maxptr = 0;
    for(unsigned int i=0; i<mlacount; i++) {
        printf("%p: %x\n", mlaptr[i].startAddr, mlaptr[i].memSize);
        if(maxptr < (unsigned long long)(mlaptr[i].startAddr) + mlaptr[i].memSize) {
            maxptr = (unsigned long long)(mlaptr[i].startAddr) + mlaptr[i].memSize;
        }
    }
    printf("maxptr:%d\n", maxptr);
    char* abuff = malloc(maxptr);
    ElfLoader_Load(fptr, abuff);

    for(unsigned int i=0; i<100; i++) {
        for(unsigned int k=0; k<16; k++) {
            printf("%d ", abuff[i*16 + k]);
        }
        printf("\n");
    }


    fclose(fptr);

    return 0;
}
