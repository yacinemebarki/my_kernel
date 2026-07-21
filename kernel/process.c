#include "types.h" 
#include "process.h"
#include "pmm.h"
#include "vga.h"

extern void restore_esp(process_t *next);


//process creating

extern int i;
extern int j;

void process_entry(){
    while (1){
        print_string("Hello from process!\n", &i, &j);
    }
}

uint16_t process_number = 1;
process_t *create_process(){
    process_t *pro = (process_t *)kmalloc(sizeof(process_t));
    if (pro == NULL) {
        return NULL;
    }
    uint32_t kernel_stack = allocate_page();

    if (kernel_stack == 0) {
        kfree((uint32_t)pro);
        return NULL;
    }

    uint32_t *stack = (uint32_t *)(kernel_stack + 4096);
    stack -= sizeof(registers_t) / sizeof(uint32_t);
    pro->regs = (registers_t *)stack;

    pro->kernel_stack = kernel_stack;
    pro->pid = process_number;
    pro->state = PROCESS_READY;
    pro->next = NULL;
    pro->regs->eip = 0;
    pro->regs->edi = 0;
    pro->regs->esi = 0;
    pro->regs->ebp = 0;
    pro->regs->ebx = 0;
    pro->regs->edx = 0;
    pro->regs->ecx = 0;
    pro->regs->eax = 0;
    pro->regs->eip = (uint32_t)process_entry;
    pro->regs->cs = 0x08;
    pro->regs->eflags = 0x202;
    process_number++;
    return pro;
}

void remove_process(process_t *pro){
    free_page(pro->kernel_stack);
    kfree((uint32_t)pro);
}

void creat_first_process(){
    process_t *pro = create_process();
    process_list = pro;
}

//process list manger
process_t *process_list = NULL;
process_t *current_process = NULL;


void add_process(process_t *pro){
    pro->next = NULL;
    process_t *current = process_list;
    if (current == NULL) {
        process_list = pro;
        return;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = pro;
}

void remove_process_list(process_t *pro) {
    process_t *current = process_list;
    process_t *previous = NULL;

    while (current != NULL) {
        if (current == pro) {
            if (previous == NULL) {
                process_list = current->next;
            } else {
                previous->next = current->next;
            }
            remove_process(current);            
            return;
        }

        previous = current;
        current = current->next;
    }
}

process_t *find_process(process_t *pro){
    process_t *current = process_list;

    while(current != NULL){
        if(current == pro){
            return pro;
        }
        current = current->next;
    }
    return NULL;
}

void save_context(registers_t *regs){
    current_process->regs = regs;
}

void context_switch(registers_t *reg, process_t *next){
    save_context(reg);
    current_process = next;  
    restore_esp(next);
}