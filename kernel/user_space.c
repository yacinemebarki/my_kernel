#include "syscall.h"

void write_string(char *str){
    __asm__ volatile("int $0x80":: "a"(SYS_PRINTS), "b"(str): "memory");
}


void user_main(){
    write_string("hello to your space");
    while(1);
}