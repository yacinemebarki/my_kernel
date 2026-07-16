#include "asm_operation.h"
#include "types.h"

extern int i;

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

void add_line(){
    unsigned short *vga = (unsigned short*)0xB8000;

    for (int row = 1; row < 25; row++) {
        for (int col = 0; col < 80; col++) {
            vga[(row - 1) * 80 + col] = vga[row * 80 + col];
        }
    }

    unsigned short blank = (0x0F << 8) | ' ';
    for (int col = 0; col < 80; col++) {
        vga[24 * 80 + col] = blank;
    }

    if (i >= 2000) {
        i -= 80;
    }
}

static inline void add_line_if_needed(int *cursor){
    if (*cursor >= 2000) {
        add_line();
    }
}

void clear(int i){
    char *line = (char *)0xB8000;
    line[2 * i] = ' ';
}

void print_number(unsigned long n,int *i){
    char buffer[21];
    int pos = 0;
    
    if(n == 0){
        add_line_if_needed(i);
        print('0', *i);
        (*i)++;
        add_line_if_needed(i);
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
        add_line_if_needed(i);
        print(buffer[counter], *i);
        (*i)++;
    }
    add_line_if_needed(i);
}

void print_time_message(char *str, int i){
    int k = 0;
    while (str[k] != '\0'){
        add_line_if_needed(&i);
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
            add_line_if_needed(i);
        } else {
            add_line_if_needed(i);
            print(str[k], *i);
            (*i)++;
            (*j)++;
            add_line_if_needed(i);
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

void print_hex(uint32_t n, int *i){
    char hex[] = "0123456789ABCDEF";

    add_line_if_needed(i);
    print('0', *i);
    (*i)++;
    add_line_if_needed(i);
    print('x', *i);
    (*i)++;

    for (int shift = 28; shift >= 0; shift -= 4){
        uint32_t digit = (n >> shift) & 0xF;
        add_line_if_needed(i);
        print(hex[digit], *i);
        (*i)++;
    }
    add_line_if_needed(i);
}
