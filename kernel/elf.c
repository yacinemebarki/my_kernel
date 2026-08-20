#include "elf.h"

int check_elf_file(Elf32_Ehdr *hdr){
    if(!hdr){
        return 0;
    }

    if(hdr->e_ident[0] != 0x7F){
        return 0;
    }
    if(hdr->e_ident[1] != 'E'){
        return 0;
    }
    if(hdr->e_ident[2] != 'L'){
        return 0;
    }
    if(hdr->e_ident[3] != 'F'){
        return 0;
    }

    return 1;
}

int check_elf_supported(Elf32_Ehdr *hdr){
    if(check_elf_file(hdr) == 0){
        return 0;
    }
    if(hdr->e_ident[4] != ELFCLASS32){
        return 0;
    }
    if(hdr->e_machine != EM_386){
        return 0;
    }
    if(hdr->e_ident[6] != EV_current){
        return 0;
    }
    if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC){
        return 0;
    }

    return 1;
}

void load_segment(Elf32_Phder *ph, void *file){
    uint32_t segment_start = ph->p_vaddr & 0xFFFFF000;
    uint32_t segment_end = (ph->p_vaddr + ph->p_memsz + 0xFFF) & 0xFFFFF000;

    for(uint32_t vaddr = segment_start; vaddr < segment_end; vaddr+=4096){
        uint32_t ver = allocate_page(PAGE_PRESENT | PAGE_WRITE | PAGE_USER);  
        if(ver == 0){
            return;
        } 
    }

    uint8_t *src = (uint8_t *)file + ph->p_offset;
    memcpy((void *)ph->p_vaddr, src, ph->p_file_sz);

    if (ph->p_memsz > ph->p_file_sz) {
        memset((uint8_t *)ph->p_vaddr + ph->p_file_sz, 0, ph->p_memsz - ph->p_file_sz);
    }
}

void *elf_load_exec(Elf32_Ehdr *hdr, void *file){
    Elf32_Phder *phdr = (Elf32_Phder *)((uint8_t *) hdr + hdr->e_phoff);

    for(uint16_t i = 0; i < hdr->e_phnum; i++){
        Elf32_Phder *ph = (Elf32_Phder *)((uint8_t *)phdr + i * hdr->e_phsize);

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