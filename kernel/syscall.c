#include "vga.h"
#include "asm_operation.h"
#include "syscall.h"
#include "pit.h"
#include "pmm.h"

//vga variable
extern int i;
extern int j;

//process variable
extern process_t *current_process;

void sys_prints(char *string){
    print_string(string, &i, &j);
}

void sys_printc(char string){
    print(string, i);
}

void sys_printn(unsigned long n){
    print_number(n, &i);
}

void sys_printhex(uint32_t n){
    print_hex(n, &i);
}


void sys_exit(int status){
    print_string("\n===== SYS_EXIT =====\n", &i, &j);

    print_string("current PID = ", &i, &j);
    print_number(current_process->pid, &i);

    print_string("\nstatus = ", &i, &j);
    print_number(status, &i);

    print_string("\n", &i, &j);

    current_process->exit_code = status;

    exit_process();

    while (1);
}

int number_switch = 0;

void sys_yield(registers_t *regs){
    process_t *old = current_process;
    wake_processes();
    process_t *next = schedule();
    number_switch++;

    if (next != NULL && next != old) {       
        context_switch(regs, next);
    }
}

void sys_print_seconds(unsigned long seconds){
    int message_pos = 0;
    int t = 0;
    print_time_message("Up Time", message_pos);
    int pos = 160;
    print_number(seconds, &pos);
}

extern int ticks;
void sys_sleep(registers_t *regs, unsigned long time){
    current_process->wake = ticks + time;
    current_process->state = PROCESS_BLOCKED;

    process_t *next = schedule();

    if (next != NULL && next != current_process) {
        context_switch(regs, next);
    }
}

process_t *sys_create_process(void (*entery)(void), int mode){
    process_t *pro = create_process(entery, mode);
    int valid = is_valid_process_ptr(pro);
    if(valid == 0){
        print_string("unvalid process", &i, &j);
        return NULL;
    }
    return pro;
}

int sys_get_pid(){
    return current_process->pid;
}

int sys_get_parent_pid(process_t *pro){
    if (!is_valid_process_ptr(pro))
        return -1;
    if (pro->parent == NULL)
        return -1;
    return pro->parent->pid;
}

uint32_t sys_kmalloc(uint32_t size){
    uint32_t pointer = kmalloc(size, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    return pointer;
}

void sys_free(uint32_t address){
    kfree(address);
}

int sys_wait(int *status, registers_t *regs){
    process_t *p = process_list;

    while(p != NULL){
        if (p->parent->pid == current_process->pid) {

            if (p->state == PROCESS_ZOMBIE) {
                *status = p->exit_code;
                return p->pid;
            }
        }
        p = p->next;
    }

    current_process->state = PROCESS_BLOCKED;
    regs->eax = -1;
    process_t *next = schedule();
    if (next == NULL || next == current_process) {
        print_string("PANIC: no runnable process\n", &i, &j);
        __asm__ volatile("cli; hlt");
    }

    context_switch(regs, next);
    return -1;
}

void syscall_dispatch(registers_t *regs){
    switch (regs->eax){
        case SYS_EXIT:
            sys_exit((int)regs->ebx);
            break;

        case SYS_YIELD:
            sys_yield(regs);
            break;

        case SYS_PRINTS:
            sys_prints((char *)regs->ebx);
            break;

        case SYS_PRINTC:
            sys_printc((char)regs->ebx);
            break;

        case SYS_PRINTN:
            sys_printn((unsigned long)regs->ebx);
            break;

        case SYS_PRINTHEX:
            sys_printhex((uint32_t)regs->ebx);
            break;
        case SYS_SECONDS:
            regs->eax = get_seconds();
            break;
        case SYS_PRINT_SECONDS:
            sys_print_seconds((unsigned long)regs->ebx);
            break;   
        case SYS_SLEEP:
            sys_sleep(regs, (unsigned long)regs->ebx);
            break;
        case SYS_CREATE_PROCESS:
            regs->eax = (uint32_t) sys_create_process((void (*)(void))regs->ebx, (int)regs->ecx);
            break;   
        case SYS_GET_PID:
            regs->eax = sys_get_pid();
            break;
        case SYS_MALLOC:
            regs->eax = sys_kmalloc(regs->ebx);
            break;
        case SYS_FREE:
            sys_free(regs->ebx);
            break;
        case SYS_WAIT:
            regs->eax = sys_wait((int *)regs->ebx, regs);
            print_string("the sys_wait result= ", &i, &j);
            print_number((int)regs->eax, &i);
            break;
        case SYS_GET_PARENT_PID:
            regs->eax = sys_get_parent_pid((process_t *)regs->ebx);
            break;

        default:
            break;
    }
}