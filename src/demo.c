#include "peripherals/mmio.h"
#include "schedule.h"
#include "uart0.h"
#include "sys.h"
#include "mm.h"

void delay(int period) {
    while (period--);
}

// Lab4: Required 1, 2

// void demo_lab4_task_1() {
//     while (1) {
//         uart_printf("%d...\n", get_current_task()->id);
//         delay(100000000);
//     }
// }

// void demo_lab4_task_2() {
//     while (1) {
//         uart_printf("%d...\n", get_current_task()->id);
//         delay(100000000);
//     }
// }

// Lab4: Required 3

// void demo_do_exec_el0() {
//     while(1) {
//         uart_printf("hello from demo_do_exec_el0\n");
//         delay(100000000);
//     }
// }

// Lab4: Required 4

// void demo_syscall_get_task_id() {
//     while(1) {
//         uart_printf("%d\n", get_taskid());
//         delay(100000000);
//     }
// }

// void demo_syscall_uart() {
//     while(1) {
//         char buf[256];
//         uart_read(buf, 1);
//         uart_write(buf, 1);
//     }
// }

// void demo_syscall_exec() {
//     exec(demo_syscall_uart);
// }

// void demo_syscall_fork() {
//     int id = fork();
//     if (id == 0) { // child
//         char buf[] = "hello from child\n";
//         while (1) {
//             uart_write(buf, 256);
//             delay(100000000);
//         }
//     }
//     else if (id > 0) {
//         char buf[] = "hello from parnt\n";
//         while (1) {
//             uart_write(buf, 256);
//             delay(100000000);
//             exit(1);
//         }
//     }
// }

// void demo_foo(){
//     int tmp = 5;
//     uart_printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
//     exit(0);
// }

// void demo_official() {
//     int cnt = 1;
//     if (fork() == 0) {
//         fork();
//         delay(100000);
//         fork();
//         while(cnt < 10) {
//             uart_printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
//             delay(100000);
//             ++cnt;
//         }
//         exit(0);
//         uart_printf("Should not be printed\n");
//     } else {
//         uart_printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
//         exec(demo_foo);
//     }
// }

// void demo_lab4_do_exec() {
//     do_exec(demo_official);
// }

// Lab5: Required 2-2, 2-3

// void demo_lab5_req2() {
//     while (1) {
//         void* ptr = page_alloc();
//         uint64_t virt = (uint64_t) ptr;
//         uint64_t phy = virtual_to_physical(virt);
//         uint64_t pfn = phy_to_pfn(phy);
//         uart_printf("%x %x %x...\n", virt, phy, pfn);
//         page_free(ptr);
//         delay(10000000);
//     }
// }

// Lab6

void demo_lab6() {
    // buddy_info();
    // void* addr1 = kmalloc(4097);
    // uart_printf("%x\n", addr1);
    // buddy_info();
    // kfree(addr1);
    // buddy_info();
    buddy_info();
    void* addr1 = buddy_alloc(2);
    uart_printf("%x\n", addr1);
    buddy_info();
    kfree(addr1);
    buddy_info();
    while(1);
}

// Lab7

void demo_lab7() {
    void* addr1 = kmalloc(1);
    void* addr2 = kmalloc(2);
    void* addr3 = kmalloc(3);
    uart_printf("%x\n", addr1);
    uart_printf("%x\n", addr2);
    uart_printf("%x\n", addr3);
    while(1);
}
