#include "types.h"
#include "asm_operation.h"
#include "vga.h"
#include "pmm.h"

#define MAX_FREE_PAGES 1024

uint32_t free_virtual[MAX_FREE_PAGES];
uint32_t free_count = 0;

uint32_t next_virtual = 0xC0000000;
extern int i;
extern int j;

uint32_t page_table[1024] __attribute__((aligned(4096)));
uint32_t page_directory[1024] __attribute__((aligned(4096)));

uint16_t *entry = (uint16_t *)0x0500;
Memory_Map *map = (Memory_Map *)0x504;

void search(int i, int j){
    print_string("Entries: ", &i, &j);
    print_number(*entry, &i);
    i = i + 80;
    for(int k = 0; k < *entry; k++){
        unsigned long long start = map[k].base;
        unsigned long long size = map[k].length;
        unsigned int type = map[k].type;
        print_string("base: ", &i, &j);
        print_number(start, &i);
        print_string("Size: ", &i, &j);
        print_number(size, &i);
        print_string("Type: ", &i, &j);
        print_number(type, &i);
        i = i + 80;
    }
}

uint32_t allocate(unsigned long long size){
    int best = -1;

    for(int k = 0; k < *entry; k++){
        if(map[k].base == 0){
            continue;
        }
        if(map[k].length >= size && map[k].type == 1){
            if(best == -1 || map[best].length > map[k].length){
                best = k;
            }
        }
    }
    if( best == -1){
        return 0;
    }

    unsigned long long remain = map[best].length - size;
    
    if(remain > 0){
        for(int k = *entry; k > best; k--){
            map[k] = map[k -1];
        }
        map[best + 1].base = map[best].base + size;
        map[best + 1].length = remain;
        map[best + 1].type = 1;
        map[best + 1].acpi = map[best].acpi;
        (*entry)++;
    }
    map[best].length = size;
    map[best].type = 2;

    return (uint32_t)map[best].base;
}

void free(unsigned long long add){
    int index = -1;

    for (int k = 0; k < *entry; k++){
        if (map[k].base == add){
            index = k;
            break;
        }
    }

    if (index == -1)
        return;

    map[index].type = 1;

    if (index > 0 &&map[index - 1].type == 1 && map[index - 1].base + map[index - 1].length == map[index].base){
        map[index - 1].length += map[index].length;

        for (int k = index; k < *entry - 1; k++){
            map[k] = map[k + 1];
        }

        (*entry)--;
        index--;
    }
    if (index < *entry - 1 && map[index + 1].type == 1 && map[index].base + map[index].length == map[index + 1].base){
        map[index].length += map[index + 1].length;

        for (int k = index + 1; k < *entry - 1; k++){
            map[k] = map[k + 1];
        }
       (*entry)--;
    }
}

void build_first_page(){
    for(int k = 0; k < 1024; k++){
        page_table[k] = (k * 0x1000) | 3;
    }
    page_directory[0] = ((uint32_t)page_table) | 3;

    for (int k = 1; k < 1024; k++){
        page_directory[k] = 0;
    }
}

void map_page(uint32_t physical_address, uint32_t virtual_address, unsigned int flags){
    uint32_t directory = virtual_address >> 22;
    uint32_t table = (virtual_address >> 12 ) & 0x3FF;

    if ((page_directory[directory] & 1) == 0){
        uint32_t new_table = allocate(4096);
        if (new_table == 0) {
            print_string("allocate FAILED\n", &i, &j);
            return;
        }
        uint32_t *table_ptr = (uint32_t *)new_table;
        print_string("new_table addr = ", &i, &j);
        print_number((uint32_t)new_table, &i);
        print_string("\npage_table addr = ", &i, &j);
        print_number((uint32_t)page_table, &i);
        print_string("\n", &i, &j);;
               
        for (int i = 0; i < 1024; i++){
            table_ptr[i] = 0;
        }
        page_directory[directory] = (uint32_t)new_table | 3;
    }

    uint32_t *table_ptr = (uint32_t *)(page_directory[directory] & 0xFFFFF000);
    table_ptr[table] = (physical_address & 0xFFFFF000) | flags;
}

uint32_t allocate_page(){
    uint32_t physical = allocate(4096);
    if (physical == 0)
        return 0;

    uint32_t virtual;
    if(free_count > 0){
        virtual = free_virtual[--free_count];
    }else{
        virtual = next_virtual;
        next_virtual += 4096;
    }
    
    map_page(physical, virtual, 3);
    return virtual;
}

void free_page(uint32_t addr){
    free_virtual[free_count++] = addr;
    uint32_t directory = addr >> 22;
    uint32_t table = (addr >> 12) & 0x3FF;

    uint32_t *table_ptr = (uint32_t *)(page_directory[directory] & 0xFFFFF000);

    uint32_t physical = table_ptr[table] & 0xFFFFF000;
    free(physical);
     table_ptr[table] = 0;
}