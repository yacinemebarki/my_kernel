#include "keyboard.h"
#include "vga.h"
#include "idt.h"
#include "types.h"

extern void keyboard_isr();

struct idt_pointer ptr;
struct interrupt_des idt[256];

int i = 640;
int space = 80;
int j = 0;

static inline void outb(uint16_t port, uint8_t value){
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void remap_pic(void){
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

__attribute__((used, externally_visible))
void keyboard_handler(void){
    char c = read();

    if (c == 0)
        return;

    if (c == '\n') {
        i = i + space;
        i = i - j;
        move(i);
        j = 0;
    }else if (c == ' ') {
        i++;
        j++;
        move(i);
    }else if (c == '\b') {
        i--;
        j--;
        clear(i);
    }else {
        print(c, i);
        i++;
        j++;
    }
}

void kernel(){
    ptr.limit = sizeof(idt) - 1;
    ptr.base = (uint32_t)&idt;
    remap_pic();
    outb(0x21, 0xFD);
    add(33, (uint32_t)keyboard_isr, idt);
    load_idt(&ptr);
    __asm__ volatile("sti");

    while (1){
    }
}
