#include "user_space.h"

extern unsigned char _binary_user_program2_elf_start[];

void _start() {
    sys_prints("I am program 1\n");

    user_exec(_binary_user_program2_elf_start);

    write_string("EXEC FAILED\n");

    while (1) {}
}