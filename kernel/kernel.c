//imports from other files
#include "keyboard.h"
#include "vga.h"
#include "idt.h"
#include "types.h"
#include "pit.h"
#include "asm_operation.h"
#include "pmm.h"
#include "tests.h"
#include "process.h"

//define
#define HZ 100
#define TIME_POS 160
#define up_pos 0
#define start_pos 240


//avoid ide error
extern void keyboard_isr();
extern void irq0();
extern void load_page_directory(uint32_t *directory);
extern void enable_paging(void);
extern void isr8();
extern void isr13();
extern void isr14();

//idt attribute table and pointer
struct idt_pointer ptr;
struct interrupt_des idt[256];

//screen atribute
int i = 400;
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
    if(i >= 2000){
        add_line();
    }
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
    }else if(c == '2'){
        i = i + 80;
        if(i >= 2000){
            add_line();
        }
        move(i);
    }else if(c == '1'){
        i = i - 80;
        move(i);
    }else if(c == '3'){
        i--;
        move(i);
    }else if(c == '4'){
        i++;
        if(i >= 2000){
            add_line();
        }
        move(i);
    }
    else {
        print(c, i);
        i++;
        j++;
        move(i);
    }
    outb(0x20, 0x20);
}
//time handler

volatile unsigned long ticks = 0;

void irq0_handler(registers_t *regs){ 
    ticks++;  
    outb(0x20, 0x20);                

    if(current_process == NULL || process_list == NULL){
        return;                        
    }

    wake_processes();
    process_t *next = schedule();
    if(next != current_process){
        context_switch(regs, next);
    }
}

void fault_handler(uint32_t vector){
    print_string("FAULT: vector ", &i, &j);
    print_hex(vector, &i);
    __asm__ volatile("cli; hlt");
}

void get_seconds(void){
    unsigned long second = ticks / HZ;
    int pos = TIME_POS;
    print_number(second, &pos);
}

void sleep_process(int time){
    current_process->state = PROCESS_BLOCKED;
    current_process->wake = ticks + time; 
}

void sleep(int time){
    int start = ticks;
    while(ticks < time + start);
}

void uptime_task(void){
    unsigned long last = (unsigned long)-1;
    while (1){
        unsigned long sec = ticks / 100;
        if (sec != last){
            last = sec;
            print_time_message("Up Time", up_pos);
            get_seconds();
        }
    }
}

//test kernel function

void test_sleep(void){
    print_string("sleepy: about to sleep\n", &i, &j);

    unsigned long start = ticks;
    sleep_process(2000);           
    while (current_process->state != PROCESS_READY) {
        __asm__ volatile("hlt");
    }

    print_string("sleepy: woke up! ticks elapsed = ", &i, &j);
    print_number(ticks - start, &i);
    print_string("\n", &i, &j);

}



void kernel(){
    //fix the screen
    clear_screen();
    print_string("hello world\n", &i, &j);
    print_time_message("start os", start_pos);
    
    print_string("Before paging \n", &i, &j);

    build_first_page();
    print_string("\n", &i, &j);
    load_page_directory(page_directory);
    enable_paging();

    //add the interruptions
    ptr.limit = sizeof(idt) - 1;
    ptr.base = (uint32_t)&idt;
    remap_pic();
    outb(0x21, 0xFC);
    add(33, (uint32_t)keyboard_isr, idt);
    add(32, (uint32_t)irq0, idt);
    add(8,  (uint32_t)isr8,  idt);
    add(13, (uint32_t)isr13, idt);
    add(14, (uint32_t)isr14, idt);
    load_idt(&ptr);
    __asm__ volatile("sti");

    //init pit
    pit_init(11931);
    unsigned long last = 0;
    unsigned long test = 0;
    process_t *p1 = create_process(uptime_task);
    process_t *p2 = create_process(test_sleep);
    process_list = p1;
    p1->next = p2;
    current_process = p1;

    restore_esp(p1);   
}
