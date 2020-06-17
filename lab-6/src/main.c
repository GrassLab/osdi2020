#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "time.h"
#include "command.h"
#include "task.h"
#include "printf.h"
#include "list.h"
#include "memory.h"
#include "allocator.h"

void test_lab6_req1() {
    page_sys_init();

    print_buddy();
    printf("------\n");

    Page *p = get_page(5);
    print_buddy();
    printf("------\n");

    Page *p2 = get_page(3);
    print_buddy();
    printf("------\n");

    Page *p3 = get_page(2);
    print_buddy();
    printf("------\n");

    Page *p4 = get_page(2);
    print_buddy();
    printf("------\n");

    Page *p5 = get_page(2);
    print_buddy();
    printf("------\n");

    free_page(p2);
    print_buddy();
    printf("------\n");

    free_page(p3);
    print_buddy();
    printf("------\n");

    free_page(p4);
    print_buddy();
    printf("------\n");

    free_page(p5);
    print_buddy();
    printf("------\n");

    free_page(p);
    print_buddy();
    printf("------\n");
}

void test_lab6_req2() {
    page_sys_init();
    allocator_init();
    
    allocator_register(1200);
    allocator_register(40);

    Slub *s1 = allocate_object(40);
    Slub *s2 = allocate_object(1200);
    Slub *s3 = allocate_object(1200);
    Slub *s4 = allocate_object(40);
    Slub *s5 = allocate_object(1200);
    Slub *s6 = allocate_object(40);
    Slub *s7 = allocate_object(1200);

    free_object(s3);
    free_object(s1);
    free_object(s2);
    free_object(s4);
    free_object(s5);
    free_object(s6);
    free_object(s7);

    allocator_register(128);
    Slub *s8 = allocate_object(128);
    free_object(s8);
}

void test_lab6_req3() {
    page_sys_init();
    allocator_init();
    varied_size_init();
    unsigned long m1 = allocate_memory(1023);
    unsigned long m2 = allocate_memory(1024);
    unsigned long m3 = allocate_memory(1025);
    unsigned long m4 = allocate_memory(3);
    unsigned long m5 = allocate_memory(1);
    unsigned long m6 = allocate_memory(9999);
    free_memory(m1);
    free_memory(m2);
    free_memory(m3);
    free_memory(m4);
    free_memory(m5);
    free_memory(m6);

    unsigned long m7 = allocate_memory(1023);
    free_memory(m7);
}

int main()
{
    uart_init();
    init_printf(0, putc);
    // test_lab6_req1();
    // test_lab6_req2();
    test_lab6_req3();
}
