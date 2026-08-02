#include "vga.h"

extern int i;
extern int j;

void user_main(){
    while(1){
        __asm__ volatile("int $0x80");
    }
}