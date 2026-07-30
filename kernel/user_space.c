#include "vga.h"

extern int i;
extern int j;

void user_main(){
    print_string("hello user", &i, &j);
    while(1){
        __asm__ volatile("int $0x80");
    }
}