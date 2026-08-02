#include "vga.h"
#include "process.h"
#include "asm_operation.h"

//vga variable
extern int i;
extern int j;

//process variable
extern process_t *current_process;

void write_string(char *string){
    print_string(string, &i, &j);
}

void sys_exit(int status){
    current_process->exit_code = status;
    exit_process();
    while (1);
}

void sys_yield(registers_t *regs){
    process_t *next = schedule();
    if(next != current_process){
        context_switch(regs, next);
    }
}

void user_main(){
    write_string("hello to your space");
    __asm__ volatile("int $0x80");
}