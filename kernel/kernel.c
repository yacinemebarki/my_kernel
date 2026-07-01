//imports from other files
#include "keyboard.h"
#include "vga.h"
#include "idt.h"
#include "types.h"
#include "pit.h"
#include "asm_operation.h"

//define
#define HZ 100
#define TIME_POS 240
#define up_pos 0
#define start_pos 480


//avoid ide error
extern void keyboard_isr();
extern void irq0();

//idt attribute table and pointer
struct idt_pointer ptr;
struct interrupt_des idt[256];

//screen atribute
int i = 640;
int space = 80;
int j = 0;

//pic remap


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
//handler

//keyboard handler
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
        move(i);
    }else {
        print(c, i);
        i++;
        j++;
        move(i);
    }
    outb(0x20, 0x20);
}
//time handler

volatile unsigned long ticks = 0;

void irq0_handler(){ 
    ticks++;  
    outb(0x20, 0x20);
}

void get_seconds(void){
    unsigned long second = ticks / HZ;
    print_number(second, TIME_POS);
}


void kernel(){
    //fix the screen
    clear_screen();
    print_string("hello world", &i, &j);
    print_time_message("Up Time", up_pos);
    print_time_message("start os", start_pos);

    //add the interruptions
    ptr.limit = sizeof(idt) - 1;
    ptr.base = (uint32_t)&idt;
    remap_pic();
    outb(0x21, 0xFC);
    add(33, (uint32_t)keyboard_isr, idt);
    add(32, (uint32_t)irq0, idt);
    load_idt(&ptr);
    __asm__ volatile("sti");

    //init pit
    pit_init(11931);
    unsigned long last = 0;


    while (1){
        //time track
        unsigned long sec = ticks / 100;
        if (sec != last){
            last = sec;
            get_seconds();
        }
    }
}
