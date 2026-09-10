#include "user_space.h"

void _start(){
    write_string("Hello from user space!\n");
    user_exit(0);
}