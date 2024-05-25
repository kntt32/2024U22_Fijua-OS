#include <elf.h>
#include "elfloader.h"

#define NULL ((void*)0)


static void ElfLoader_MemCopy(const void* from, const void* to) {
    
    return;
}



unsigned int ElfLoader_Load(const void* file, void* loadAddr) {
    if(file == NULL) return 1;

    Elf_Header* elfHeader = (Elf_Header*)file;
    Elf_ProgramHeader* programHeader = (void*)(elfHeader->e_phoffset + (uintn)file);
    uint16 programHeaderSize = elfHeader->e_phentsize;
    uint16 programHeaderNumber = elfHeader->e_phnum;

    switch(elfHeader->e_type) {
        case Elf_Header_ET_EXEC:
            for(unsigned int i=0; i<programHeaderNumber; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    //code here...
                }

                programHeader = (void*)((uintn)programHeader + programHeaderSize);
            }
            break;
        case Elf_Header_ET_DYN:
            break;
        default:
            return 2;
    }
    
    return 0;
}



unsigned int ElfLoader_GetLoadArea(const void* file, const void* loadAddr, ElfLoader_MemLoadArea* buff, uintn* buffCount) {
    if(file == NULL || buff == NULL || *buffCount == 0) return 1;

    Elf_Header* elfHeader = (Elf_Header*)file;
    Elf_ProgramHeader* programHeader = (void*)(elfHeader->e_phoffset + (uintn)file);
    uint16 programHeaderSize = elfHeader->e_phentsize;
    uint16 programHeaderNumber = elfHeader->e_phnum;

    if(*buffCount < programHeaderNumber) {
        *buffCount = programHeaderNumber;
        return 1;
    }

    *buffCount = 0;

    switch(elfHeader->e_type) {
        case Elf_Header_ET_EXEC:
            for(unsigned int i=0; i<programHeaderNumber; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    buff[*buffCount].startAddr = (void*)(programHeader->p_vaddr);
                    buff[*buffCount].memSize = programHeader->p_memsz;

                    (*buffCount)++;
                }

                programHeader = (void*)((uintn)programHeader + programHeaderSize);
            }
            break;
        case Elf_Header_ET_DYN:
            for(unsigned int i=0; i<programHeaderNumber; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    buff[*buffCount].startAddr = (void*)(programHeader->p_vaddr + loadAddr);
                    buff[*buffCount].memSize = programHeader->p_memsz;

                    (*buffCount)++;
                }

                programHeader = (void*)((uintn)programHeader + programHeaderSize);
            }
            break;
        default :
            return 2;
    }
    
    return 0;
}
