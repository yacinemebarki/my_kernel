#include "user_space.h"

void user_malloc_free_test(void){
    write_string("USER MALLOC/FREE TEST\n");

    uint32_t ptr = user_kmalloc(64);
    if (ptr == 0) {
        write_string("malloc failed\n");
        return;
    }

    write_string("allocated addr=");
    write_hex(ptr);
    write_char('\n');

    user_free(ptr);
    write_string("freed\n");
}

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