#include "asm_operation.h"

void move(unsigned short pos){
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0x0FF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

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

void print_number(unsigned long n,int i){
    char buffer[20];
    
    if(n == 0){
        print('0', i);
        i++;
        return;
    }
    int counter = 0;
    while (n > 0){
        buffer[i++] = (n % 10) + '0';
        n /= 10;
        counter++;
    }
    while (counter > 0){
        counter--;  
        print(buffer[counter], i);
        i++;
    }
    

}

