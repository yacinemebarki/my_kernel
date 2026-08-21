#ifndef ELF_H
#define ELF_H

#include "types.h"
#include "pmm.h"
#include "vga.h"
#include "process.h"

#define ELFCLASS32 1
#define ELFCLASS64 2

#define EM_386 3

#define EV_CURRENT 1

#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6

extern int i;
extern int j;


typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
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


typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;


void *elf_load_file(void *file);
process_t *create_elf_process(void *file);

#endif