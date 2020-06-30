#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "printf.h"
#include "memory.h"
#include "allocator.h"
#include "fat32.h"
#include "vfs.h"

#define NULL 0

void test_lab8_req1() {
    sd_init();
    // setup_mount();
}

void test_lab8_req2() {

}

int main()
{
    uart_init();
    init_printf(0, putc);
    // page_sys_init();
    // allocator_init();
    // varied_size_init();
    test_lab8_req1();
}
