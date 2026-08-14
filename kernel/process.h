#ifndef PROCESS_H
#define PROCESS_H
#include "types.h"

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS 0x1B
#define USER_DS 0x23

#define PROCESS_KERNEL 0
#define PROCESS_USER 1

#define KERNEL_STACK_SIZE 4096
#define SAVED_REGS_OFFSET 0x100

typedef enum{
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED,
    PROCESS_ZOMBIE
} process_state_t;

typedef struct registers{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

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

    uint32_t user_esp;
    uint32_t user_ss;
} registers_t;

typedef struct process{
    uint32_t pid;
    uint32_t *page_directory;
    uint32_t kernel_stack;
    uint32_t kernel_stack_to;
    registers_t *regs;
    process_state_t state;
    int exit_code;
    void (*entry)(void);
    int wake;
    struct process *next;
    struct process *parent;
} process_t;

extern process_t *process_list;
extern uint16_t process_number;
extern process_t *current_process;

process_t *create_process(void (*entry)(void), int mode);
void add_process(process_t *pro);
void remove_process(process_t *pro);
void remove_process_list(process_t *pro);
process_t *find_process(process_t *pro);
void save_context(registers_t *regs);
void context_switch(registers_t *regs, process_t *next);
process_t *schedule();
extern void restore_esp(process_t *next);
void process_test();
void exit_process();
void process_entry();
void wake_processes(void);
int is_valid_process_ptr(process_t *pro);

#endif