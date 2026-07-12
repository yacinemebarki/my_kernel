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
}

void clear(int i){
    char *line = (char *)0xB8000;
    line[2 * i] = ' ';
}

void print_number(unsigned long n,int *i){
    char buffer[21];
    int pos = 0;
    
    if(n == 0){
        print('0', *i);
        (*i)++;
        return;
    }
    int counter = 0;
    while (n > 0){
        buffer[pos++] = (n % 10) + '0';
        n /= 10;
        counter++;
    }
    while (counter > 0){
        counter--;  
        print(buffer[counter], *i);
        (*i)++;
    }
}

void print_time_message(char *str, int i){
    int k = 0;
    while (str[k] != '\0'){
        print(str[k], i);
        k++;
        (i)++;
    }
}

void print_string(char *str, int *i, int *j) {
    int k = 0;

    while (str[k] != '\0') {
        if (str[k] == '\n') {
            *i += 80 - (*i % 80);
            *j = 0;
        } else {
            print(str[k], *i);
            (*i)++;
            (*j)++;
        }

        k++;
    }

    move(*i);
}

void clear_screen(){
    unsigned short *vga = (unsigned short*)0xB8000;
    for(int i = 0; i < 2000; i++){
        vga[i] = 0x0720;
    }
}

