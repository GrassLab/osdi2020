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

    allocate_object(40);
    allocate_object(1200);
    allocate_object(1200);
    allocate_object(40);
    allocate_object(40);
    allocate_object(40);
    allocate_object(40);
    allocate_object(1200);   
    allocate_object(1200);
}

void test_lab6_req3() {
    
}

int main()
{
    uart_init();
    init_printf(0, putc);
    // test_lab6_req1();
    test_lab6_req2();
}
