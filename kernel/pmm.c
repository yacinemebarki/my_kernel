#include "types.h"
#include "asm_operation.h"
#include "vga.h"

typedef struct {
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;    
} Memory_Map;

void search(int i, int j){
    uint16_t *entry = (uint16_t *)0x500;
    Memory_Map *map = (Memory_Map *)0x504;

    for(int k = 0; k < *entry; k++){
        if(map[k].type == 1){
            unsigned long start = map[k].base;
            unsigned long size = map[k].length;
            print_string("base: ", &i, &j);
            print_number(start, i);
            print_string("Size: ", &i, &j);            
            print_number(size, i);
        }
    }
}