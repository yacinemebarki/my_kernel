#ifndef USER_SPACE_H
#define USER_SPACE_H

#include "process.h"

void write_string(char *str);
void write_char(char c);
void write_number(unsigned long n);
void write_hex(uint32_t n);
void write_second(uint32_t n);
unsigned long user_get_second(void);
void user_up_time(void);
void user_yield(void);
void user_exit(int status);
process_t *user_create_process(void (*entry)(void), int mode);
void user_sleep(int time);
uint32_t user_kmalloc(uint32_t size);
void user_free(uint32_t address);
void user_malloc_free_test(void);
void user_main(void);
int user_wait(int *status);

#endif