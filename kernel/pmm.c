#include "types.h"
#include "asm_operation.h"
#include "vga.h"
#include "pmm.h"

#define MAX_FREE_PAGES 1024

//define heap variable
uint32_t page_table[1024] __attribute__((aligned(4096)));
uint32_t page_directory[1024] __attribute__((aligned(4096)));

Block *heap_head = NULL;
Block *current_block = NULL;

uint32_t next_virtual = 0xC0000000;
uint32_t free_virtual[MAX_FREE_PAGES];
uint32_t free_count = 0;


//define free page array

extern int i;
extern int j;

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
        print_hex((uint32_t)start, &i);
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
    if (best == -1){
        return 0;
    }

    unsigned long long remain = map[best].length - size;
    
    if(remain > 0){
        if (*entry >= MAX_FREE_PAGES) {
            return 0;
        }

        for(int k = *entry; k > best; k--){
            map[k] = map[k - 1];
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

void free(uint32_t physical_address){
    int index = -1;

    for (int k = 0; k < *entry; k++){
        if (map[k].base == physical_address){
            index = k;
            break;
        }
    }

    if (index == -1)
        return;

    map[index].type = 1;

    if (index > 0 && map[index - 1].type == 1 && map[index - 1].base + map[index - 1].length == map[index].base){
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
        /*
        print_string("\nnew_table addr = ", &i, &j);
        print_number((uint32_t)new_table, &i);
        print_string("\npage_table addr = ", &i, &j);
        print_number((uint32_t)page_table, &i);
        print_string("\n", &i, &j);;
        */               
        for (int k = 0; k < 1024; k++){
            table_ptr[k] = 0;
        }
        page_directory[directory] = (uint32_t)new_table | 3;
    }

    uint32_t *table_ptr = (uint32_t *)(page_directory[directory] & 0xFFFFF000);
    table_ptr[table] = (physical_address & 0xFFFFF000) | flags;
    invlpg(virtual_address);
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

uint32_t *get_pte(uint32_t virtual){
    uint32_t directory = virtual >> 22;
    uint32_t table = (virtual >> 12) & 0x3FF;

    if (!(page_directory[directory] & 1))
        return NULL;

    uint32_t *table_ptr =
        (uint32_t *)(page_directory[directory] & 0xFFFFF000);

    return &table_ptr[table];
}

uint32_t get_map(uint32_t virtual){
    uint32_t *pte = get_pte(virtual);

    if (pte == NULL || !(*pte & 1))
        return 0;

    return *pte & 0xFFFFF000;
}

void free_page(uint32_t addr){
    free_virtual[free_count++] = addr;

    uint32_t *pte = get_pte(addr);

    if (pte == NULL || !(*pte & 1))
        return;

    free(*pte & 0xFFFFF000);
    *pte = 0;
}

void unmap_page(uint32_t virtual){
    uint32_t *pte = get_pte(virtual);
    if (pte == NULL || !(*pte & 1))
        return;
    *pte = 0;
    invlpg(virtual);
}

uint32_t kmalloc(uint32_t size){
    if (size > 4096 || size == 0)
        return 0;

    if (heap_head == NULL) {
        heap_head = current_block = (Block *)allocate_page();
        heap_head->size = 4096 - sizeof(Block);
        heap_head->type = 1;
        heap_head->next = NULL;
    }

    Block *best_block = NULL;
    Block *current_page = heap_head;
    print_string("\nheap size = ", &i, &j);
    print_number(heap_head->size, &i);
    while (current_page != NULL){
        if(current_page->type == 1 && current_page->size >= size){

            if(best_block == NULL || best_block->size > current_page->size ){
                best_block = current_page;
            }
        }
        current_page = current_page->next;
    }
    if(best_block == NULL){
        Block *new_page = (Block *) allocate_page();        
        new_page->size = 4096 - sizeof(Block);
        new_page->type = 1;
        new_page->next = NULL;
        current_block->next = new_page;
        best_block = new_page;
        current_block = new_page;
    }

    if(best_block->size >= size + sizeof(Block)){
        Block *new_block = (Block *)((char *)best_block + sizeof(Block) + size);
        new_block->next = best_block->next;
        new_block->size = best_block->size - size - sizeof(Block);
        new_block->type = 1;
        best_block->size = size;
        best_block->type = 2;
        best_block->next = new_block;
        return (uint32_t)((char *)best_block + sizeof(Block));
    }
    best_block->type = 2;   
    return (uint32_t)((char *)best_block + sizeof(Block));

}
void kfree(uint32_t addr){
    Block *current_page = heap_head;
    while(current_page != NULL){
        uint32_t user_addr = (uint32_t)((uint8_t *)current_page + sizeof(Block));
        if(user_addr == addr){
            current_page->type = 1;
            return;
        }
        current_page = current_page->next;
    }
}

void inspect(){
    Block *cur = heap_head;

    while (cur != NULL) {
        print_string("\nBlock: ", &i, &j);
        print_hex((uint32_t)cur, &i);

        print_string(" size=", &i, &j);
        print_number(cur->size, &i);

        print_string(" type=", &i, &j);
        print_number(cur->type, &i);

        cur = cur->next;
    }
}