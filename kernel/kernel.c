#include "keyboard.h"

void print(char c, int i){
    char *line = (char *)0xB8000;
    line[2 * i] = c;
    line[2 * i + 1] = 0x0F;
}

void kernel(){
    int i = 640;
    int space = 80;
    while (1){
        char c = read();
        if(c != 0){
            if(c == '\n'){
                //move cursor function will be add 
                i = i + space;
            }
            else if(c == ' '){
                //mov cursor function will be add
                i++;
            }
            else if(c == '\b'){
                //clear_char function will be add
                i--;
            }
            else {
                print(c, i);
                i++;
            }
        }
    }
    
}
