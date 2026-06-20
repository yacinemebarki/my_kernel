#ifndef IDT_H
#define IDT_H
#include "types.h"

struct interrupt_des {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zeros;
    uint8_t type;
    uint16_t offset_2;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void add(int n, uint32_t handler, struct interrupt_des *idt);
void load_idt(struct idt_pointer *ptr);

#endif