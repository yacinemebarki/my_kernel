#include "types.h"

struct interrupt_des{
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zeros;
    uint8_t type;
    uint16_t offset_2;
}__attribute__((packed));

struct idt_pointer{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

void add(int n, unsigned int handler, struct interrupt_des *idt){
    idt[n].offset_1 = handler & 0xFFFF;
    idt[n].selector = 0x08;
    idt[n].zeros = 0;
    idt[n].type = 0x8E;
    idt[n].offset_2 = (handler >> 16) & 0xFFFF;
}

void load_idt(struct idt_pointer *ptr){
    __asm__ volatile("lidt (%0)" : : "r"(ptr));
}

