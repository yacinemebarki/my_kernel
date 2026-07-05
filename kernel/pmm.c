#include "types.h"
#include "asm_operation.h"
#include "vga.h"
#include "pmm.h"

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

void allocate(unsigned long long size){
    
    int best = -1;

    for(int k = 0; k < *entry; k++){
        if(map[k].length >= size && map[k].type == 1){
            if(best == -1 || map[best].length > map[k].length){
                best = k;
            }
        }
    }
    if( best == -1){
        return;
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