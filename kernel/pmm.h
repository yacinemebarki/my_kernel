#ifndef PMM_H
#define PMM_H
#include "types.h"

#define MAX_FREE_PAGES 1024


typedef struct{
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;
} __attribute__((packed)) Memory_Map;

typedef struct Block{
    uint32_t size;
    uint8_t type;
    struct Block *next;
}__attribute__((packed)) Block;

extern uint32_t page_table[1024];
extern uint32_t page_directory[1024];
extern Block *heap_head;
extern Block *current_block;
extern uint32_t next_virtual;
extern uint32_t free_virtual[MAX_FREE_PAGES];
extern uint32_t free_count;

void build_first_page();
void search(int i, int j);
uint32_t allocate(unsigned long long size);
void free(uint32_t physical_address);
void map_page(uint32_t physical_address, uint32_t virtual_address, unsigned int flags);
void free_page(uint32_t addr);
uint32_t allocate_page();
uint32_t kmalloc(uint32_t size);
void kfree(uint32_t addr);
void inspect();

#endif