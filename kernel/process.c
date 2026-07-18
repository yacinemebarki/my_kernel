#include "types.h" 
#include "process.h"
#include "pmm.h"

//process list manger
process_t *process_list = NULL;

void creat_first_process(){
    process_t *pro = (process_t *)kmalloc(sizeof(process_t));
    pro->pid = 1;
    pro->state = PROCESS_READY;
    pro->next = NULL;
    process_list = pro;
}

void add_process(process_t *pro){
    pro->next = NULL;
    process_t *current_process = process_list;
    if(current_process->next == NULL){
        return;
    }
    while (current_process != NULL){
        current_process = current_process->next;
    }
    current_process->next = pro;   
}

void remove_process(process_t *pro) {
    process_t *current = process_list;
    process_t *previous = NULL;

    while (current != NULL) {
        if (current == pro) {
            if (previous == NULL) {
                process_list = current->next;
            } else {
                previous->next = current->next;
            }

            kfree(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}

process_t *find_process(process_t *pro){
    process_t *current_process = process_list;

    while(current_process != NULL){
        if(current_process == pro){
            return pro;
        }
    }
    return NULL;
}