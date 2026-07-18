#include "types.h" 
#include "process.h"
#include "pmm.h"

//process creating

uint16_t process_number = 1;
process_t *create_process(){
    process_t *pro = (process_t *)kmalloc(sizeof(process_t));
    if (pro == NULL) {
        return NULL;
    }
    uint32_t kernel_stack = allocate_page();

    if (kernel_stack == 0) {
        kfree((uint32_t)pro);
        return NULL;
    }

    pro->kernel_stack = kernel_stack;
    pro->pid = process_number;
    pro->state = PROCESS_READY;
    pro->next = NULL;
    pro->esp = kernel_stack + 4096;
    pro->ebp = pro->esp;
    pro->eip = 0;

    process_number++;
    return pro;
}

void remove_process(process_t *pro){
    free_page(pro->kernel_stack);
    kfree((uint32_t)pro);
}

void creat_first_process(){
    process_t *pro = create_process();
    process_list = pro;
}

//process list manger
process_t *process_list = NULL;
process_t *current_process = NULL;


void add_process(process_t *pro){
    pro->next = NULL;
    process_t *current = process_list;
    if (current == NULL) {
        process_list = pro;
        return;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = pro;
}

void remove_process_list(process_t *pro) {
    process_t *current = process_list;
    process_t *previous = NULL;

    while (current != NULL) {
        if (current == pro) {
            if (previous == NULL) {
                process_list = current->next;
            } else {
                previous->next = current->next;
            }
            remove_process(current);            
            return;
        }

        previous = current;
        current = current->next;
    }
}

process_t *find_process(process_t *pro){
    process_t *current = process_list;

    while(current != NULL){
        if(current == pro){
            return pro;
        }
        current = current->next;
    }
    return NULL;
}