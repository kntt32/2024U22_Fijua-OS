#ifndef INCLUDED_ELF_H
#define INCLUDED_ELF_H

#include <build.h>
#include <types.h>

#define EI_NIDENT 16

#define Elf_Header_ET_EXEC 2
#define Elf_Header_ET_DYN  3

#define Elf_Header_EM_AMD64 62


#define Elf_ProgramHeader_PT_LOAD 1

typedef struct {
    unsigned char   e_ident[EI_NIDENT]; 
    uint16      e_type;
    uint16      e_machine;
    uint32      e_version;
    uintn      e_entry;
    uintn       e_phoffset;
    uintn       e_shoffset;
    uint32      e_flags;
    uint16      e_ehsize;
    uint16      e_phentsize;
    uint16      e_phnum;
    uint16      e_shentsize;
    uint16      e_shnum;
    uint16      e_shstrndx;
} Elf_Header;

#if PLATFORM_64BIT

typedef struct {
        uint32      p_type;
        uint32      p_flags;
        uintn       p_offset;
        uintn      p_vaddr;
        uintn      p_paddr;
        uint64     p_filesz;
        uint64     p_memsz;
        uint64     p_align;
} Elf_ProgramHeader;

#else

typedef struct {
        uint32      p_type;
        uintn       p_offset;
        uintn      p_vaddr;
        uintn      p_paddr;
        uint32      p_filesz;
        uint32      p_memsz;
        uint32      p_flags;
        uint32      p_align;
} Elf_ProgramHeader;

#endif


#endif
