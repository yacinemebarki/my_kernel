#ifndef SYSCALL_H
#define SYSCALL_H
#include "process.h"

#define SYS_EXIT   0
#define SYS_YIELD  1
#define SYS_PRINTS  2
#define SYS_PRINTC 3
#define SYS_PRINTN 4
#define SYS_PRINTHEX 5
#define SYS_SLEEP  6
#define SYS_SECONDS 7
#define SYS_PRINT_SECONDS 8
#define SYS_CREATE_PROCESS 9
#define GET_PID 10
#define SYS_MALLOC 11
#define SYS_FREE 12
#define SYS_WAIT 13

void sys_yield(registers_t *regs);
void sys_exit(int status);
void sys_prints(char *string);
void syscall_dispatch(registers_t *regs);

#endif