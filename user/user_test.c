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

void user_exit_test(void){
    write_string("USER EXIT TEST\n");

    write_string("Process is running...\n");
    user_yield();

    write_string("Process is exiting now...\n");
    user_exit(0);

    write_string("ERROR: process continued after exit!\n");
}

void user_sleep_test(void){
    write_string("USER SLEEP TEST\n");

    write_string("Before sleep\n");

    uint32_t start = user_get_second();
    write_string("\nthe start is= ");
    write_number(start);
    write_string("\n");

    write_string("Sleeping for 3 seconds...\n");

    user_sleep(300);

    uint32_t end = user_get_second();

    write_string("After sleep\n");

    write_string("Elapsed time = ");
    write_number(end - start);
    write_string(" seconds\n");

    user_exit(0);
}

void user_child(void){
    write_string("CHILD RUNNING\n");
    user_exit(42);
}

void user_test_wait(void){
    write_string("USER wait TEST\n");

    process_t *p = user_create_process(user_child, PROCESS_USER);
    int ppid = user_get_parent_pid(p);
    write_string("pid of parent of child process= ");
    write_number(ppid);
    write_string("\n");

    int status;

    int pid = user_wait(&status);

    write_string("\nwait returned PID: ");
    write_number(pid);

    write_string("\nexit status: ");
    write_number(status);
    user_exit(0);
}

