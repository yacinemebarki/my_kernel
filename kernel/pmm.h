#ifndef PMM_H
#define PMM_H
#include "types.h"

typedef struct {
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;    
} __attribute__((packed))Memory_Map;

extern uint16_t *entry;
extern Memory_Map *map;
extern  page_directory[1024] __attribute__((aligned(4096)));
extern  page_table[1024] __attribute__((aligned(4096)));

void search(int i, int j);
void allocate(unsigned long long size);
void free(unsigned long long add);

#endif