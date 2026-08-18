#ifndef ELF_H
#define ELF_H

#include "types.h"

enum Elf_Type {
	ET_NONE,
	ET_REL,		
	ET_EXEC
};

#define ELFCLASS32 1
#define ELFCLASS64 2
#define EM_386 3
#define EV_current 3

enum Elfp_Type {
    
};

typedef struct{
    uint8_t e_ident[16];
    Elf_Type e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phsize;
    uint16_t e_phnum;
    uint16_t e_shsize;
    uint16_t e_shnum;
    uint16_t e_shsrtndx;

} Elf32_Ehdr;

typedef struct{
    Elfp_Type p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_phaddr;
    uint32_t p_file_sz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;    
} Elf32_Phder;


#endif