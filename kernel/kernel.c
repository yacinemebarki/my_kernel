#define VGA_MEMORY ((unsigned short*)0xb8000)

void print_char(char c,int pos){
    VGA_MEMORY[pos] = (0x0F << 8) | c;
}

void kernel(){
    print_char('h', 0);
    print_char('e', 1);
    print_char('l', 2);
    print_char('l', 3);
    print_char('o', 4);
    print_char(' ',5);
    print_char('w', 6);
    print_char('o', 7);
    print_char('r', 8);
    print_char('l', 9);
    print_char('d', 10);
    while (1);
    
}

