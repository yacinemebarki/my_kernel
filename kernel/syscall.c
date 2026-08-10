#include "vga.h"
#include "asm_operation.h"
#include "syscall.h"
#include "pit.h"

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
    current_process->exit_code = status;
    exit_process();
    while (1);
}

int number_switch = 0;

void sys_yield(registers_t *regs){
    process_t *old = current_process;
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

extern unsigned long ticks;
void sys_sleep(unsigned long time){
    unsigned long start = ticks;
    sti();
    while (ticks < start + time) {
        hlt();
    }
}

process_t *sys_create_process(void (*entery)(void), int mode){
    process_t *pro = create_process(entery, mode);
    return pro;
}

int sys_get_pid(){
    return current_process->pid;
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
            sys_sleep((unsigned long)regs->ebx);
            break;
        case SYS_CREATE_PROCESS:
            regs->eax = (uint32_t) sys_create_process((void (*)(void))regs->ebx, (int)regs->ecx);
            break;   
        case GET_PID:
            regs->eax = sys_get_pid();

        default:
            break;
    }
}