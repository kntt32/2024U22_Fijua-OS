#include <elf.h>
#include "elfloader.h"

#define NULL ((void*)0)


static void ElfLoader_MemCopy(in const void* from, in const unsigned int size, out void* to);


//fill memoy zero from start
static void ElfLoader_Bss(in out void* start, in uintn size) {
    uint8* targetptr = (uint8*)start;

    for(uintn i=0; i<size; i++) {
        *targetptr = 0;
        targetptr++;
    }

    return;
}


//compare two string
static uintn ElfLoader_CmpStr(in char str1[], in char str2[]) {
    if(str1 == NULL || str2 == NULL) return 0;

    for(uintn i=0; 1; i++) {
        if(str1[i] == '\0') return i;
        if(str1[i] != str2[i]) return 0;
    }
}


//Get property about elf file
unsigned int ElfLoader_GetProperty(in const void* file, in optional void* loadAddr, out optional void** entryPointPtr, out optional uint16* machineTypePtr) {
    if(file == NULL) return 1;

    if(entryPointPtr != NULL) {
        switch(((Elf_Header*)file)->e_type) {
            case Elf_Header_ET_EXEC:
                *entryPointPtr = (void*)(((Elf_Header*)file)->e_entry + (uintn)loadAddr);
                break;
            case Elf_Header_ET_DYN:
                *entryPointPtr = (void*)(((Elf_Header*)file)->e_entry + (uintn)loadAddr);
                break;
            default:
                return 2;
        }
    }
    if(machineTypePtr != NULL) *machineTypePtr = ((Elf_Header*)file)->e_machine;

    return 0;
}

void cac(uintn);


//Expand elf excutable file
unsigned int ElfLoader_Load(in const void* file, in uintn* loadAddr) {
    if(file == NULL) return 1;

//load elf file
    Elf_Header* elfHeader = (Elf_Header*)file;
    Elf_ProgramHeader* programHeader = (void*)(elfHeader->e_phoffset + (uintn)file);
    uint16 programHeader_entrySize = elfHeader->e_phentsize;
    uint16 programHeader_number = elfHeader->e_phnum;

    //check *loadAddr
    switch(elfHeader->e_type) {
        case Elf_Header_ET_EXEC:
            if(*loadAddr != 0) return 2;
            break;
        case Elf_Header_ET_DYN:
            break;
        default:
            return 2;
    }

    //expand elf
    for(unsigned int i=0; i<programHeader_number; i++) {
        if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
            //load
            ElfLoader_MemCopy((void*)(programHeader->p_offset+(uintn)file), programHeader->p_filesz, (void*)(programHeader->p_vaddr+(uintn)loadAddr));
            //bss section
            ElfLoader_Bss((void*)(programHeader->p_vaddr+programHeader->p_filesz+(uintn)loadAddr), programHeader->p_memsz-programHeader->p_filesz);
        }

        programHeader = (void*)((uintn)programHeader + programHeader_entrySize);
    }

//relocation
    Elf_SectionHeader* sectionHeader = (Elf_SectionHeader*)((uintn)file + elfHeader->e_shoffset);
    if(sectionHeader == NULL) return 0;//this file doesn't have section header
    uintn sectionHeader_entrySize = elfHeader->e_shentsize;
    uintn sectionHeader_number = elfHeader->e_shnum;
    uint16 sectionHeader_shStrTableIndex = elfHeader->e_shstrndx;
    uint8* sectionHeader_shStrTab = (uint8*)(((Elf_SectionHeader*)((uintn)sectionHeader + sectionHeader_entrySize*sectionHeader_shStrTableIndex))->sh_offset + (uintn)file);
    if(sectionHeader_shStrTab == NULL) return 4;//shStrTab not found

    Elf_Section_SymbolTable* symbolTable = NULL;
    uintn symbolTable_entrySize = 0;
    uint8* sectionHeader_strTab = NULL;
    //get .symtab section
    for(uintn i=0; i<sectionHeader_number; i++) {
        char* targstr = (char*)((uintn)sectionHeader_shStrTab + sectionHeader->sh_name);

        if(ElfLoader_CmpStr(".symtab", targstr)) {
            if(sectionHeader->sh_type != Elf_SectionHeader_SYMTAB) return 5;//invalid type of .symtab section
            symbolTable = (Elf_Section_SymbolTable*)(sectionHeader->sh_offset + (uintn)file);
            symbolTable_entrySize = sectionHeader->st_entsize;
        }else if(ElfLoader_CmpStr(".strtab", targstr)) {
            if(sectionHeader->sh_type != Elf_SectionHeader_STRTAB) return 6;//invalid type of .strtab section
            sectionHeader_strTab = (uint8*)(sectionHeader->sh_offset + (uintn)file);
        }

        sectionHeader = (Elf_SectionHeader*)((uintn)sectionHeader + sectionHeader_entrySize);
    }

    //relocate
    sectionHeader = (Elf_SectionHeader*)((uintn)file + elfHeader->e_shoffset);
    for(uintn i=0; i<sectionHeader_number; i++) {
        char* targstr = (char*)((uintn)sectionHeader_shStrTab + sectionHeader->sh_name);

        if(ElfLoader_CmpStr(".rela", targstr)) {
            if(sectionHeader->sh_type != Elf_SectionHeader_RELA) return 7;

            Elf_Section_Rela* relaSection = (Elf_Section_Rela*)(sectionHeader->sh_offset + (uintn)file);
            const uintn relaSection_entrySize = sectionHeader->st_entsize;
            const uintn relaSection_number = sectionHeader->sh_size / relaSection_entrySize;
            if(ElfLoader_CmpStr(".rela.text", targstr)) {
                for(uintn k=0; k<relaSection_number; k++) {
                    uint64 relaType = Elf_RelRela_Type(relaSection->r_info);
                    uint64 symtabIndex = Elf_RelRela_Sym(relaSection->r_info);

                    uint64 A = relaSection->r_append;//elf64_section_rela.append
                    uint64 B = *loadAddr;
                    

                    switch(relaType) {
                        default:
                            return 8;//unsupported rela type
                    }

                    relaSection = (Elf_Section_Rela*)((uintn)relaSection + relaSection_entrySize);
                }
            }else {
                for(uintn k=0; k<relaSection_number; k++) {

                }
            }
        }else if(ElfLoader_CmpStr(".rel", targstr)) {
            if(sectionHeader->sh_type != Elf_SectionHeader_REL) return 8;

            Elf_Section_Rel* relSection = (Elf_Section_Rel*)(sectionHeader->sh_offset + (uintn)file);
            const uintn relSection_entrySize = sectionHeader->st_entsize;
            const uintn relSection_number = sectionHeader->sh_size / relSection_entrySize;
            if(ElfLoader_CmpStr(".rel.text", targstr)) {

            }else {

            }
        }


        sectionHeader = (Elf_SectionHeader*)((uintn)sectionHeader + sectionHeader_entrySize);
    }
while(1) {}

    
    return 0;
}



unsigned int ElfLoader_GetLoadArea(in void* file, in out uintn* loadAddr, out uintn* size) {
    if(file == NULL || loadAddr == NULL || size == NULL) return 1;

    const Elf_ProgramHeader* programHeader = (const Elf_ProgramHeader*)(((uintn)(((Elf_Header*)file)->e_phoffset)) + (uintn)(file));
    const uintn programHeader_entrySize = (uintn)(((Elf_Header*)file)->e_phentsize);
    const uintn programHeader_number = (uintn)(((Elf_Header*)file)->e_phnum);
    if(programHeader == NULL) return 2;

    *size = 0;
    switch(((Elf_Header*)file)->e_type) {
        case Elf_Header_ET_DYN:
            for(uintn i=0; i<programHeader_number; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    if(*size < programHeader->p_offset+programHeader->p_memsz) *size = programHeader->p_offset+programHeader->p_memsz;
                }
                programHeader = (Elf_ProgramHeader*)((uintn)(programHeader) + programHeader_entrySize);
            }
            break;
        case Elf_Header_ET_EXEC:
            *loadAddr = 0;
            for(uintn i=0; i<programHeader_number; i++) {
                if(programHeader->p_type == Elf_ProgramHeader_PT_LOAD) {
                    if(*size < programHeader->p_offset+programHeader->p_memsz) *size = programHeader->p_offset+programHeader->p_memsz;
                }
                programHeader = (Elf_ProgramHeader*)((uintn)(programHeader) + programHeader_entrySize);
            }
            break;
        default:
            return 3;
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







