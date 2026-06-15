void kernel(){
    char* vga = (char*)0xB8000;

    vga[0] = 'H';
    vga[1] = 0x0F;
}
