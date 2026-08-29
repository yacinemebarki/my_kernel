#include "types.h" 
#include "process.h"
#include "pmm.h"
#include "vga.h"
#include "tss.h"

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
    process_t *pro = (process_t *)kmalloc(sizeof(process_t), PAGE_PRESENT | PAGE_WRITE);

    if (pro == NULL)
        return NULL;

    uint32_t kernel_stack = allocate_page(PAGE_PRESENT | PAGE_WRITE);

    if (kernel_stack == 0) {
        kfree((uint32_t)pro);
        return NULL;
    }

    uint32_t user_stack = 0;

    if (mode == PROCESS_USER) {
        user_stack = allocate_page(PAGE_PRESENT | PAGE_WRITE | PAGE_USER);

        if (user_stack == 0) {
            free_page(kernel_stack);
            kfree((uint32_t)pro);
            return NULL;
        }
        pro->user_stack = user_stack;
    }
    registers_t *regs = (registers_t *)(kernel_stack + KERNEL_STACK_SIZE - sizeof(registers_t));

    pro->regs = regs;
    pro->kernel_stack = kernel_stack;
    pro->kernel_stack_to = kernel_stack + KERNEL_STACK_SIZE;
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

        regs->esp = kernel_stack + KERNEL_STACK_GAP;
    } else {

        regs->gs = USER_DS;
        regs->fs = USER_DS;
        regs->es = USER_DS;
        regs->ds = USER_DS;

        regs->eip = (uint32_t)entry;
        regs->cs = USER_CS;
        regs->eflags = 0x202;

        regs->user_esp = user_stack + 4096;
        regs->user_ss = USER_DS;
    }

    pro->parent = current_process;
    
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
    if (!current_process || !current_process->regs)
        return;

    current_process->regs->gs = regs->gs;
    current_process->regs->fs = regs->fs;
    current_process->regs->es = regs->es;
    current_process->regs->ds = regs->ds;

    current_process->regs->edi = regs->edi;
    current_process->regs->esi = regs->esi;
    current_process->regs->ebp = regs->ebp;
    current_process->regs->esp = regs->esp;

    current_process->regs->ebx = regs->ebx;
    current_process->regs->edx = regs->edx;
    current_process->regs->ecx = regs->ecx;
    current_process->regs->eax = regs->eax;

    current_process->regs->eip = regs->eip;
    current_process->regs->cs = regs->cs;
    current_process->regs->eflags = regs->eflags;

    if ((regs->cs & 3) == 3) {
        current_process->regs->user_esp = regs->user_esp;
        current_process->regs->user_ss = regs->user_ss;
    }
    print_string("\nSAVE PID = ", &i, &j);
    print_hex(current_process->pid, &i);

    print_string("\nsource CS = ", &i, &j);
    print_hex(regs->cs, &i);

    print_string("\nsaved CS = ", &i, &j);
    print_hex(current_process->regs->cs, &i);

    print_string("\nsource EIP = ", &i, &j);
    print_hex(regs->eip, &i);

    print_string("\nsaved EIP = ", &i, &j);
    print_hex(current_process->regs->eip, &i);
}

void context_switch(registers_t *reg, process_t *next){
    /*
    print_string("\n--- SWITCH ---\n", &i, &j);

    print_string("incoming regs = ", &i, &j);
    print_hex((uint32_t)reg, &i);

    print_string("\nincoming EIP = ", &i, &j);
    print_hex(reg->eip, &i);

    print_string("\nincoming CS = ", &i, &j);
    print_hex(reg->cs, &i);

    print_string("\nnext regs = ", &i, &j);
    print_hex((uint32_t)next->regs, &i);

    print_string("\nnext EIP = ", &i, &j);
    print_hex(next->regs->eip, &i);

    print_string("\nnext CS = ", &i, &j);
    print_hex(next->regs->cs, &i);
    */

    save_context(reg);
    /*
    print_string("\nAFTER SAVE\n", &i, &j);

    print_string("next CS = ", &i, &j);
    print_hex(next->regs->cs, &i);
    */
    current_process = next;

    print_string("\nBEFORE RESTORE\n", &i, &j);

    print_string("restore CS = ", &i, &j);
    print_hex(next->regs->cs, &i);
   
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

    dead->state = PROCESS_ZOMBIE;

    if (dead->parent != NULL) {
        dead->parent->state = PROCESS_READY;
    }

    process_t *next = schedule();

    if (next == NULL || next == dead) {
        print_string("PANIC: no ready process after exit\n", &i, &j);
        __asm__ volatile("cli; hlt");
    }

    if (next != current_process) {
        current_process = next;
    }
    restore_esp(next);

    while (1);
}

void process_entry(){
    current_process->entry();   
    exit_process();             
}

int is_valid_process_ptr(process_t *pro){
    process_t *p = process_list;
    while (p != NULL){
        if (p == pro) return 1;
        p = p->next;
    }
    return 0;
}