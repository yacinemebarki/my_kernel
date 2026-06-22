#ifndef VGA_H
#define VGA_H

void print(char c, int i);
void clear(int i);
void move(unsigned short port);
void print_number(unsigned long n, int i);

#endif