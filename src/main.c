#include <stdint.h>

#include "alloc.h"
#include "io.h"
#include "page.h"
#include "pool.h"
#include "syscall.h"
#include "task.h"
#include "tests.h"
#include "uart.h"
#include "vfs.h"

extern volatile unsigned int _boot_start;
extern uint64_t _end;
extern uint64_t _pte_base;
extern void set_aux();
extern uint64_t _binary_user_img_start;
extern uint64_t _binary_user_img_end;

void idle() {
    if (runqueue.head->next == 0) {
        print_s("No task");
        while (1)
            ;
    } else {
        schedule();
    }
}

void zombie_killer() {
    print_s("this is killer\n");
    while (1) {
        for (int i = 0; i < 64; i++) {
            if (task_pool[i].status == ZOMBIE) {
                task_pool[i].status = INACTIVE;
                break;
            }
        }
    }
}

void foo_kernel() {
    init_rootfs();
    char buf[100];
    struct file* a = vfs_open("hello", O_CREAT);
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World\n", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    print_s(buf);
    mkdir("dir1");

    struct file* root = vfs_open("/", 0);
    vfs_list(root);

    chdir("dir1");
    mkdir("dir2");

    struct file* dir = vfs_open(".", 0);
    vfs_list(dir);

    while (1)
        ;
}

void foo() { privilege_task_create(foo_kernel, 0); }

int main() {
    // set up serial console
    uart_init();
    task_init();
    set_aux();
    buddy_init();
    // init core timer
    asm volatile("svc #2");

    privilege_task_create(idle, 3);
    foo();
    privilege_task_run();
}
