#include "elf.h"

int check_elf_file(Elf32_Ehdr *hdr){
    if(!hdr){
        return 0;
    }

    if(hdr->e_ident[0] != 0x7F){
        print_string("index 0 error \n", &i, &j);
        return 0;
    }
    if(hdr->e_ident[1] != 'E'){
        print_string("index 1 error \n", &i, &j);
        return 0;
    }
    if(hdr->e_ident[2] != 'L'){
        print_string("index 2 error \n", &i, &j);
        return 0;
    }
    if(hdr->e_ident[3] != 'F'){
        print_string("index 3 error \n", &i, &j);
        return 0;
    }

    return 1;
}

int check_elf_supported(Elf32_Ehdr *hdr){
    if(check_elf_file(hdr) == 0){
        return 0;
    }
    if(hdr->e_ident[4] != ELFCLASS32){
        print_string("index 4 error \n", &i, &j);
        return 0;
    }
    if(hdr->e_machine != EM_386){
        print_string("machine error \n", &i, &j);
        return 0;
    }
    if(hdr->e_ident[6] != EV_CURRENT){
        print_string("index 6 error \n", &i, &j);
        return 0;
    }
    if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC){
        print_string("not existing type \n", &i, &j);
        return 0;
    }
    print_string("succesed check \n", &i, &j);
    return 1;
}

void load_segment(Elf32_Phdr *ph, void *file){
    uint32_t segment_start = ph->p_vaddr & 0xFFFFF000;
    uint32_t segment_end = (ph->p_vaddr + ph->p_memsz + 0xFFF) & 0xFFFFF000;

    for(uint32_t vaddr = segment_start; vaddr < segment_end; vaddr+=4096){
        uint32_t phy = allocate(4096);
        if(phy == 0){
            return;
        } 
        map_page(phy, vaddr, PAGE_PRESENT | PAGE_USER | PAGE_WRITE);
    }

    uint8_t *src = (uint8_t *)file + ph->p_offset;
    memcpy((void *)ph->p_vaddr, src, ph->p_filesz);

    if (ph->p_memsz > ph->p_filesz) {
        memset((uint8_t *)ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
    }
}

void *elf_load_exec(Elf32_Ehdr *hdr, void *file){
    Elf32_Phdr *phdr = (Elf32_Phdr *)((uint8_t *) hdr + hdr->e_phoff);

    for(uint16_t i = 0; i < hdr->e_phnum; i++){
        Elf32_Phdr *ph = (Elf32_Phdr *)((uint8_t *)phdr + i * hdr->e_phsize);

        if(ph->p_type != PT_LOAD){
            continue;
        }
        load_segment(ph, file);
    }
    return (void *)hdr->e_entry;
}

void *elf_load_rel(Elf32_Ehdr *hdr){
}

void *elf_load_file(void *file){
    Elf32_Ehdr *hdr = (Elf32_Ehdr *) file;

    if(check_elf_supported(hdr) == 0){
        return NULL;
    }
    switch(hdr->e_type){
        case ET_EXEC:
            return elf_load_exec(hdr, file);
        case ET_REL:
            return elf_load_rel(hdr);
        default:
            return NULL;
    }

}

process_t *create_elf_process(void *file){
    process_t *p = create_process(NULL, PROCESS_USER);
    if(p == NULL){
        print_string("process creation faild", &i, &j);
        return NULL;
    }
    uint32_t *entry = (uint32_t *) elf_load_file(file);

    if (entry != NULL) {
        print_string("ELF loaded successfully\n", &i, &j);
        print_hex((uint32_t)entry, &i);
    } else {
        print_string("ELF loading FAILED\n", &i, &j);
    }

    p->regs->eip = (uint32_t )entry;

    uint32_t phy = allocate(4096);

    map_page(phy, USER_STACK_TOP - 4096, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    p->regs->user_esp = USER_STACK_TOP;
    return p;
}