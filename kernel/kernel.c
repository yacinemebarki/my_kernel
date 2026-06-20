#include "keyboard.h"
#include "vga.h"

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
