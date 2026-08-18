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