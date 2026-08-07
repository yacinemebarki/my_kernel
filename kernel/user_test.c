#include "user_space.h"

void child_process(void){
    while (1) {
        write_string("CHILD\n");
        user_yield();
    }
}

void parent_process(void){
    write_string("PARENT: creating child...\n");

    process_t *child = user_create_process(child_process, PROCESS_USER);

    write_string("PARENT: child created!\n");

    while (1) {
        write_string("PARENT\n");
        user_yield();
    }
}