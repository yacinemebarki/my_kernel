#include "tests.h"
#include "vga.h"
#include "pmm.h"

extern int i;
extern int j;

void test_paging(void) {
    print_string("\nTEST: paging\n", &i, &j);
    build_first_page();
    print_string("page_directory[0] = ", &i, &j);
    print_hex(page_directory[0], &i);
    print_string("\n", &i, &j);
}

void test_allocation(void) {
    print_string("\nTEST: alloc\n", &i, &j);
    uint32_t addr = allocate(4096);
    print_string("alloc addr = ", &i, &j);
    print_hex(addr, &i);
    print_string("\n", &i, &j);
}

void test_kmalloc(void) {
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

void test_kfree(void) {
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

void run_tests(void) {
    test_paging();
    test_allocation();
    test_kmalloc();
    test_kfree();
}
