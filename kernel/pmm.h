#ifndef PMM_H
#define PMM_H
#include "types.h"

typedef struct {
    unsigned long long base;
    unsigned long long length;
    unsigned int type;
    unsigned int acpi;    
} Memory_Map;
void search(int i, int j);

#endif