#include <elf.h>
#include "elfloader.h"

#define NULL ((void*)0)


static void ElfLoader_MemCopy(in const void* from, in const unsigned int size, out void* to);


unsigned int ElfLoader_GetProperty(in const void* file, optional void** entryPointPtr, optional uint16* machineTypePtr) {
    if(file == NULL) return 1;

    if(entryPointPtr != NULL) *entryPointPtr = (void*)(((Elf_Header*)file)->e_entry);
    if(machineTypePtr != NULL) *machineTypePtr = ((Elf_Header*)file)->e_machine;

    return 0;
}


unsigned int ElfLoader_Load(in const void* file, in optional void* loadAddr) {
    if(file == NULL) return 1;

    Elf_Header* elfHeader = (Elf_Header*)file;
    Elf_ProgramHeader* programHeader = (void*)(elfHeader->e_phoffset + (uintn)file);
    uint16 programHeaderSize = elfHeader->e_phentsize;
    uint16 programHeaderNumber = elfHeader->e_phnum;

    switch(elfHeader->e_type) {
        case Elf_Header_ET_EXEC:
            for(unsigned int i=0; i<programHeaderNumber; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    ElfLoader_MemCopy((void*)(programHeader->p_offset+(uintn)file), programHeader->p_memsz, (void*)(programHeader->p_vaddr));
                }

                programHeader = (void*)((uintn)programHeader + programHeaderSize);
            }
            break;
        case Elf_Header_ET_DYN:
            for(unsigned int i=0; i<programHeaderNumber; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    ElfLoader_MemCopy((void*)(programHeader->p_offset+(uintn)file), programHeader->p_memsz, (void*)(programHeader->p_vaddr+(uintn)loadAddr));
                }

                programHeader = (void*)((uintn)programHeader + programHeaderSize);
            }
            break;
        default:
            return 2;
    }
    
    return 0;
}



unsigned int ElfLoader_GetLoadArea(in const void* file, in optional const void* loadAddr, in out uintn* buffCount, out ElfLoader_MemLoadArea* buff) {
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
        default:
            return 2;
    }
    
    return 0;
}



static void ElfLoader_MemCopy(in const void* from, in const unsigned int size, out void* to) {
    if(from == NULL || to == NULL || size == 0) return;

    uint8* fromptr = (void*)from;
    uint8* toptr = (void*)to;
    unsigned int sizedec = size;

#if PLATFORM_64BIT
    uint64* fromptr64 = (void*)fromptr;
    uint64* toptr64 = (void*)toptr;
    for(unsigned int i=0; i<(size >> 3); i++) {
        *toptr64 = *fromptr64;
        toptr64++;
        fromptr64++;

        sizedec -= 8;
    }
    fromptr = (uint8*)fromptr64;
    toptr = (uint8*)toptr64;
#else
    uint32* fromptr32 = (void*)fromptr;
    uint32* toptr32 = (void*)toptr;
    for(unsigned int i=0; i<(size >> 2); i++) {
        *toptr32 = *fromptr32;
        toptr32++;
        fromptr32++;

        sizedec -= 4;
    }
    fromptr = (uint8*)fromptr32;
    toptr = (uint8*)toptr32;
#endif
    for(unsigned int i=0; i<sizedec; i++) {
        *toptr = *fromptr;
        toptr++;
        fromptr++;
    }

    return;
}







