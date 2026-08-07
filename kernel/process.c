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

void process_test(){
    int count = 0;
    while (1){
        print_string("starint process", &i, &j);
    }
}

uint16_t process_number = 1;
process_t *create_process(void (*entry)(void), int mode){
    process_t *pro = (process_t *)kmalloc(sizeof(process_t));

    if (pro == NULL)
        return NULL;

    uint32_t kernel_stack = allocate_page();

    if (kernel_stack == 0) {
        kfree((uint32_t)pro);
        return NULL;
    }

    uint32_t user_stack = 0;

    if (mode == PROCESS_USER) {
        user_stack = allocate_page();

        if (user_stack == 0) {
            free_page(kernel_stack);
            kfree((uint32_t)pro);
            return NULL;
        }
    }

    registers_t *regs = (registers_t *)(kernel_stack + 4096 - 128);

    pro->regs = regs;
    pro->kernel_stack = kernel_stack;
    pro->pid = process_number++;
    pro->state = PROCESS_READY;
    pro->wake = 0;
    pro->next = NULL;
    pro->entry = entry;
    memset(regs, 0, sizeof(registers_t));
    if (mode == PROCESS_KERNEL) {

        regs->gs = KERNEL_DS;
        regs->fs = KERNEL_DS;
        regs->es = KERNEL_DS;
        regs->ds = KERNEL_DS;

        regs->eip = (uint32_t)process_entry;
        regs->cs = KERNEL_CS;
        regs->eflags = 0x202;

        regs->esp = kernel_stack + 4096;

    } else {

        regs->gs = USER_DS;
        regs->fs = USER_DS;
        regs->es = USER_DS;
        regs->ds = USER_DS;

        regs->eip = (uint32_t)process_entry;
        regs->cs = USER_CS;
        regs->eflags = 0x202;

        regs->user_esp = user_stack + 4096;
        regs->user_ss = USER_DS;
    }

    add_process(pro);

    return pro;
}

void remove_process(process_t *pro){
    free_page(pro->kernel_stack);
    kfree((uint32_t)pro);
}

void add_process(process_t *pro){
    pro->next = NULL;
    process_t *current = process_list;
    if (current == NULL) {
        process_list = pro;
        current_process = pro;
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
    if (current_process != NULL && current_process->regs != NULL) {

        print_string("SAVE PID=", &i, &j);
        print_number(current_process->pid, &i);

        print_string(" regs=", &i, &j);
        print_hex((uint32_t)regs, &i);

        print_string(" stored=", &i, &j);
        print_hex((uint32_t)current_process->regs, &i);

        print_string(" EIP=", &i, &j);
        print_hex(regs->eip, &i);

        print_string(" CS=", &i, &j);
        print_hex(regs->cs, &i);

        print_string("\n", &i, &j);

        *current_process->regs = *regs;
    }
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
    return NULL;
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

void exit_process(){
    process_t *dead = current_process;
    process_t *next = schedule();

    if (next == NULL || next == dead) {        
        print_string("PANIC: no ready process after exit\n", &i, &j);
        __asm__ volatile("cli; hlt");
    }

    
    current_process->state = PROCESS_TERMINATED;
    remove_process_list(current_process);

    current_process = next;
    restore_esp(next);

}

void process_entry(){
    current_process->entry();   
    exit_process();             
}