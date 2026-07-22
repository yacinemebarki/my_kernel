#include "tests.h"
#include "vga.h"
#include "pmm.h"
#include "process.h"

extern int i;
extern int j;

void test_paging(void){
    print_string("\nTEST: paging\n", &i, &j);
    build_first_page();
    print_string("page_directory[0] = ", &i, &j);
    print_hex(page_directory[0], &i);
    print_string("\n", &i, &j);
}

void test_allocation(void){
    print_string("\nTEST: alloc\n", &i, &j);
    uint32_t addr = allocate(4096);
    print_string("alloc addr = ", &i, &j);
    print_hex(addr, &i);
    print_string("\n", &i, &j);
}

void test_kmalloc(void){
    print_string("\nTEST: kmalloc\n", &i, &j);
    uint32_t a = kmalloc(100);
    uint32_t b = kmalloc(200);
    uint32_t c = kmalloc(300);

    print_string("a=", &i, &j);
    print_hex(a, &i);
    print_string("\nb=", &i, &j);
    print_hex(b, &i);
    print_string("\nc=", &i, &j);
    print_hex(c, &i);
    print_string("\n", &i, &j);

    inspect();
}

void test_kfree(void){
    print_string("\nTEST: kfree\n", &i, &j);
    uint32_t a = kmalloc(100);
    uint32_t b = kmalloc(200);

    print_string("a=", &i, &j);
    print_hex(a, &i);
    print_string("\nb=", &i, &j);
    print_hex(b, &i);
    print_string("\nfree a\n", &i, &j);
    kfree(a);
    uint32_t c = kmalloc(50);
    print_string("c=", &i, &j);
    print_hex(c, &i);
    print_string("\n", &i, &j);
}

void test_page_alloc_free(void){
    print_string("\nTEST: page alloc/free\n", &i, &j);

    uint32_t vaddr = allocate_page();
    print_string("allocated virtual=", &i, &j);
    print_hex(vaddr, &i);
    print_string("\n", &i, &j);

    uint32_t phys = get_map(vaddr);
    print_string("mapped phys=", &i, &j);
    print_hex(phys, &i);
    print_string("\n", &i, &j);

    free_page(vaddr);
    uint32_t unmapped = get_map(vaddr);
    print_string("\nafter free get_map=", &i, &j);
    print_hex(unmapped, &i);
    print_string("\n", &i, &j);

    if (unmapped == 0){
        print_string("OK\n", &i, &j);
    } else {
        print_string("FAIL\n", &i, &j);
    }
}

void test_mapping(void){
    print_string("\nTEST: mapping\n", &i, &j);

    uint32_t physical = allocate(4096);
    if (physical == 0){
        print_string("allocate failed\n", &i, &j);
        return;
    }

    uint32_t virtual_address = 0xC0100000;
    map_page(physical, virtual_address, 3);

    print_string("mapped virtual=", &i, &j);
    print_hex(virtual_address, &i);
    print_string(" phys=", &i, &j);
    print_hex(physical, &i);
    print_string("\n", &i, &j);

    uint32_t mapped = get_map(virtual_address);
    print_string("get_map returns ", &i, &j);
    print_hex(mapped, &i);
    if (mapped != physical){
        print_string(" MISMATCH\n", &i, &j);
    }
    else{
        print_string(" OK\n", &i, &j);
    }

    unmap_page(virtual_address);
    uint32_t unmapped = get_map(virtual_address);
    print_string("after unmap get_map=", &i, &j);
    print_hex(unmapped, &i);
    print_string("\n", &i, &j);

    free(physical);
}

void test_process_list(void){
    print_string("\nTEST: process list\n", &i, &j);

    process_list = NULL;
    process_number = 1;

    creat_first_process();
    process_t *first = process_list;
    if(first == NULL){
        print_string("\n faild", &i, &j);
    }
    print_string("\nfirst pid=", &i, &j);
    print_number(first->pid, &i);
    print_string("\n", &i, &j);

    process_t *second = create_process();
    if(second == NULL){
        print_string("\n faild", &i, &j);
    }
    add_process(second);
    print_string("second pid=", &i, &j);
    print_number(second->pid, &i);
    print_string("\n", &i, &j);

    process_t *found = find_process(second);
    if (found == second) {
        print_string("find_process OK\n", &i, &j);
    } else {
        print_string("find_process FAIL\n", &i, &j);
    }

    remove_process_list(second);
    print_string("removed second\n", &i, &j);

    if (find_process(second) == NULL) {
        print_string("remove confirmed\n", &i, &j);
    } else {
        print_string("remove failed\n", &i, &j);
    }
    print_string("\n", &i, &j);
}

void test_save_context(void){
    print_string("\nTEST: Save Context\n", &i, &j);

    process_t *p = create_process();

    current_process = p;

    registers_t regs;

    regs.eip = 0x12345678;
    regs.esp = 0xAAAABBBB;
    regs.ebp = 0xCCCCDDDD;

    save_context(&regs);

    print_string("Saved regs pointer = ", &i, &j);
    print_hex((uint32_t)current_process->regs, &i);

    print_string("\nExpected pointer = ", &i, &j);
    print_hex((uint32_t)&regs, &i);

    print_string("\n", &i, &j);
}

void test_first_process(void){
    print_string("\nTEST: First Process\n", &i, &j);

    creat_first_process();

    print_string("Starting process...\n", &i, &j);

    restore_esp(current_process);
   
}

void test_scheduler(void){
    print_string("\nTEST: Scheduler\n", &i, &j);

    process_list = NULL;
    current_process = NULL;

    process_t *p1 = create_process();
    process_t *p2 = create_process();
    process_t *p3 = create_process();

    process_list = p1;
    p1->next = p2;
    p2->next = p3;
    p3->next = NULL;

    p1->state = PROCESS_RUNNING;
    p2->state = PROCESS_READY;
    p3->state = PROCESS_READY;

    current_process = p1;

    process_t *next = schedule();

    print_string("Current PID = ", &i, &j);
    print_number(current_process->pid, &i);

    print_string("\nNext PID = ", &i, &j);
    print_number(next->pid, &i);

    print_string("\nExpected = ", &i, &j);
    print_number(p2->pid, &i);

    print_string("\n", &i, &j);
    remove_process_list(p1);
    remove_process_list(p2);
    remove_process_list(p3);
    current_process = NULL;
    process_list = NULL;
}

void run_tests(void){
    test_paging();
    test_allocation();
    test_kmalloc();
    test_kfree();
    test_page_alloc_free();
    test_process_list();
    test_mapping();
}
