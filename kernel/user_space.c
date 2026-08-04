#include "syscall.h"
#include "asm_operation.h"

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

void user_sleep(int time){
    __asm__ volatile("int $0x080":: "a"(SYS_SLEEP), "b"(time));
}

void user_main(){
    write_string("hello to your space");
    user_sleep(300);
    user_up_time();
}