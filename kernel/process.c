#include "types.h" 
#include "process.h"
#include "pmm.h"
#include "vga.h"

//process creating

extern int i;
extern int j;
extern int ticks;

//process list manger
process_t *process_list = NULL;
process_t *current_process = NULL;

void process_entry(){
    int count = 0;
    while (1){
        print_string("starint process", &i, &j);
    }
}

uint16_t process_number = 1;
process_t *create_process(void (*entry) (void)){
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
    pro->wake = 0;
    pro->next = NULL;
    pro->regs->eip = 0;
    pro->regs->edi = 0;
    pro->regs->esi = 0;
    pro->regs->ebp = 0;
    pro->regs->ebx = 0;
    pro->regs->edx = 0;
    pro->regs->ecx = 0;
    pro->regs->eax = 0;
    pro->regs->eip = (uint32_t) entry;
    pro->regs->cs = 0x08;
    pro->regs->eflags = 0x202;
    process_number++;
    return pro;
}

void remove_process(process_t *pro){
    free_page(pro->kernel_stack);
    kfree((uint32_t)pro);
}

void creat_first_process(void (*entry) (void)){
    process_t *pro = create_process(entry);
    process_list = pro;
    current_process = pro;
}



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

process_t *schedule(){
    process_t *next = current_process->next;

    while(next != NULL){
        if(next->state == PROCESS_READY ){
            return next;
        }
        next = next->next;
    }

    next = process_list;

    while(next != current_process){
        if(next->state == PROCESS_READY){
            return next;
        }        
        next = next->next;
    }
    return next;
}

void wake_processes(void){
    process_t *p = process_list;

    while (p != NULL) {
        if (p->state == PROCESS_BLOCKED &&ticks >= p->wake){
            p->state = PROCESS_READY;
        }
        p = p->next;
    }
}