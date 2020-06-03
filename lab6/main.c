#include "mm.h"
#include "system.h"
#include "uart.h"
#include "util.h"

#define CMD_HELP "help"
#define CMD_HELLO "hello"
#define CMD_TIME "timestamp"
#define CMD_REBOOT "reboot"

void get_timestamp();

void main()
{
    uart_init();
    init_memory();

    void* test1 = malloc(sizeof(free_area_t));
    void* test2 = malloc(sizeof(free_area_t));
    void* test3 = malloc(sizeof(free_area_t));
    // void *test4 = malloc(sizeof(free_area_t));
    // void *test5 = malloc(sizeof(free_area_t));
    // void *test6 = malloc(sizeof(free_area_t));
    // void *test7 = malloc(sizeof(free_area_t));

    free_memory(test1);
    free_memory(test2);
    free_memory(test3);
    // free_memory(test4);
    // free_memory(test5);
    // free_memory(test6);
    // free_memory(test7);

    void* test8 = malloc((0x1 << 23) + 1000);
    free_memory(test8);

    print_variable_memory_pool();
    void* test9 = variable_malloc(4);
    print_variable_memory_pool();
    variable_free(test9);
    print_variable_memory_pool();
}

void get_timestamp()
{
    register unsigned long f, c;
    asm volatile("mrs %0, cntfrq_el0"
                 : "=r"(f));
    asm volatile("mrs %0, cntpct_el0"
                 : "=r"(c));
    char res[30];
    ftoa(((float)c / (float)f), res, 10);
    uart_puts(res);
}
