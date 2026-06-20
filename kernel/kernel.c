#include "keyboard.h"
#include "vga.h"
#include "idt.h"
#include "types.h"

struct idt_pointer ptr;
struct interrupt_des idt[256];

int i = 640;
int space = 80;
int j = 0;

void keyboard_handler(){
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
    load_idt(&ptr);

    while (1){
        keyboard_handler();
    }
    
}
