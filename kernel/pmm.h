#ifndef PMM_H
#define PMM_H
#include "types.h"

typedef struct{
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;
} __attribute__((packed)) Memory_Map;

extern uint16_t *entry;
extern Memory_Map *map;
extern uint32_t page_directory[1024] __attribute__((aligned(4096)));
extern uint32_t page_table[1024] __attribute__((aligned(4096)));

void build_first_page();
void search(int i, int j);
uint32_t allocate(unsigned long long size);
void free(uint32_t physical_address);
void map_page(uint32_t physical_address, uint32_t virtual_address, unsigned int flags);
void free_page(uint32_t addr);
uint32_t allocate_page();

#endif