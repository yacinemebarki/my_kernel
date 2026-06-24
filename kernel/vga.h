#ifndef VGA_H
#define VGA_H

void print(char c, int i);
void clear(int i);
void move(unsigned short port);
void print_number(unsigned long n, int i);
void print_string(char *str, int *i);
void clear_screen();
void print_time_message(char *str, int i);


#endif