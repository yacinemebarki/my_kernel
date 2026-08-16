#include "syscall.h"
#include "asm_operation.h"
#include "user_test.h"
#include "user_space.h"

void write_string(char *str){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINTS), "b"(str): "memory");
}

void write_char(char c){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINTC), "b"((int)c): "memory");
}

void write_number(unsigned long n){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINTN), "b"(n): "memory");
}

void write_hex(uint32_t n){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINTHEX), "b"(n): "memory");
}

void write_second(uint32_t n){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINT_SECONDS), "b"(n): "memory");
}

unsigned long user_get_second(){
    unsigned long result;
    __asm__ volatile("int $0x80": "=a"(result): "a"(SYS_SECONDS): "memory");
    return result;
}

void user_up_time(){
    unsigned long last = -1;
    unsigned sec = 0;
    write_string("Up Time");
    while (1){
        sec = user_get_second();
        if(last != sec){
            write_second(sec);
            last = sec;
        }
        
    }
}

void user_yield(){
    __asm__ volatile("int $0x080":: "a"(SYS_YIELD));
}

void user_exit(int status){
    __asm__ volatile("int $0x080":: "a"(SYS_EXIT), "b"(status));
}

process_t *user_create_process(void(*entry)(void), int mode){
    process_t *pro;
    __asm__ volatile("int $0x080":"=a"(pro): "a"(SYS_CREATE_PROCESS), "b"(entry), "c"(mode));
    return pro;
}

void user_sleep(int time){
    __asm__ volatile("int $0x080":: "a"(SYS_SLEEP), "b"(time));
}

int user_get_pid(){
    int pid;
    __asm__ volatile("int $0x080":"=a"(pid):"a"(SYS_GET_PID): "memory");
    return pid;
}

int user_get_parent_pid(process_t *pro){
    int ppid;
    __asm__ volatile("int $0x080":"=a"(ppid) :"a"(SYS_GET_PARENT_PID), "b"(pro));
    return ppid;
}

uint32_t user_kmalloc(uint32_t size){
    uint32_t pointer;
    __asm__ volatile("int $0x080":"=a"(pointer) :"a"(SYS_MALLOC), "b"(size));
    return pointer;
}

void user_free(uint32_t address){
    __asm__ volatile("int $0x080":: "a"(SYS_FREE), "b"(address));
}

int user_wait(int *status){
    int pid;
    do {
        __asm__ volatile("int $0x080":"=a"(pid): "a"(SYS_WAIT), "b"(status));
    } while (pid == -1);
    return pid;
}

int user_fork(){
    int pid;
    __asm__ volatile("$int 0x080": "=a"(pid): "a"(SYS_FORK));
    return pid;
}

void user_main(){
    write_string("hello to your space");
    process_t *p1 = user_create_process(user_up_time, PROCESS_USER);
    while(1){     
        user_yield();
    }
}