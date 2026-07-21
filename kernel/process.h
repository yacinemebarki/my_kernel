#ifndef PROCESS_H
#define PROCESS_H
#include "types.h"

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} registers_t;

typedef struct process{
    uint32_t pid;
    uint32_t *page_directory;
    uint32_t kernel_stack;
    uint32_t kernel_stack_to;
    registers_t *regs;
    process_state_t state; 
    struct process *next;   
} process_t;

extern process_t *process_list;
extern uint16_t process_number;
extern process_t *current_process;

process_t *create_process();
void creat_first_process();
void add_process(process_t *pro);
void remove_process(process_t *pro);
void remove_process_list(process_t *pro);
process_t *find_process(process_t *pro);
void save_context(registers_t *regs);


#endif