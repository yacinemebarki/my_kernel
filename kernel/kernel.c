#include "keyboard.h"

void print(char c, int i){
    char *line = (char *)0xB8000;
    line[2 * i] = c;
    line[2 * i + 1] = 0x0F;
    move(i+1);
}

void clear(int i){
    char *line = (char *)0xB8000;
    line[2 * i] = ' ';
    move(i);
}

static inline void outb(unsigned short port, unsigned char value){
    __asm__ volatile("outb %0, %1"::"a"(value),"Nd"(port));
}

void move(unsigned short pos){
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0x0FF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

void kernel(){
    int i = 640;
    int space = 80;
    int j = 0;
    while (1){
        char c = read();
        if(c != 0){
            if(c == '\n'){
                i = i + space;
                i = i - j;
                move(i);
                j = 0;
            }
            else if(c == ' '){
                i++;
                j++;
                move(i+1);
            }
            else if(c == '\b'){
                i--;
                clear(i);  
            }
            else {
                print(c, i);
                i++;
                j++;
            }
        }
    }
    
}
