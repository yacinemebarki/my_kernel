#include "tss.h"
#include "pmm.h"
#include "GTD.h"

tss_t tss;
struct gdt_entry gdt[6];
struct gdt_ptr gp;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access = access;
}

void init_tss(void){
    gdt_set_gate(5, (uint32_t)&tss, sizeof(tss) - 1, 0x89, 0x40);
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x90000;
    tss.iomap_base = sizeof(tss);
}

void init_gdt(void){
    gdt_set_gate(0, 0, 0, 0, 0);                 
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); 
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF3, 0xCF);  

    init_tss();   

    gp.limit = sizeof(gdt) - 1;
    gp.base = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gp);
    tss_flush();
}

void set_kernel_stack(process_t *next){
    tss.esp0 = next->kernel_stack + 4096;
}