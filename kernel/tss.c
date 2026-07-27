#include "tss.h"
#include "pmm.h"
#include "GTD.h"

tss_t tss;

void init_tss(void){
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x90000;
    tss.iomap_base = sizeof(tss);
}

void set_kernel_stack(uint32_t stack){
    tss.esp0 = stack;
}