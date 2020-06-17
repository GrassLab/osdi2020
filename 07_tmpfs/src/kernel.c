#include "peripherals/uart.h"
#include "peripherals/irq.h"
#include "entry.h"
#include "utils.h"
#include "timer.h"
#include "fork.h"
#include "vfs.h"
#include "sys.h"
#include "mm.h"

const unsigned int delay_interval = 100000000;

void foo() {
    while(1) {
        uart_puts("Task ID: ");
        uart_send_ulong(current -> task_id);
        uart_puts("; task_struct ptr: ");
        uart_send_hex((unsigned long)current);
        uart_puts("; sp: ");
        uart_send_hex(current -> cpu_context.sp);
        uart_send('\n');
        delay(delay_interval);
        schedule();
    }
}

void test() {
    int cnt = 1;
    if (fork() == 0) {
        fork();
        delay(delay_interval);
        fork();
        while (cnt < 10) {
            uart_puts("Task id: ");
            uart_send_ulong(current->task_id);
            uart_puts(", cnt: ");
            uart_send_ulong(cnt);
            uart_send('\n');
            delay(delay_interval);
            ++ cnt;
        }
        exit(0);
    } else {
        while (1) {
            delay(delay_interval);
        }
    }
}

void create_user_process() {
    int ret = do_exec((unsigned long)&test);
    if (ret) {
        uart_send_ulong(ret);
        uart_puts("Create user process failed.\n");
    }
}

void idle() {
    while(1){
        schedule();
        delay(delay_interval);
    }
}

void zombie_reaper(){
    while(1){
        delay(10000);
        struct task_struct *t;
        for (int i = 0; i < NR_TASKS; ++i) {
            t = tasks[i];
            if(t && t->state == TASK_ZOMBIE){
                free_task_struct((unsigned long)t);
                tasks[i] = 0;
            }
        }
    }
}

void demo_memory() {
    show_buddy_system();
    void* addrr0 = get_free_pages(PAGE_AVAILABLE, 6);
    void* addrr1 = get_free_pages(PAGE_AVAILABLE, 6);
    void* addrr2 = get_free_pages(PAGE_AVAILABLE, 6);
    void* addrr3 = get_free_pages(PAGE_AVAILABLE, 6);
    show_buddy_system();
    put_free_pages(addrr0, 6);
    put_free_pages(addrr2, 6);
    show_buddy_system();
    put_free_pages(addrr1, 6);
    put_free_pages(addrr3, 6);

    uart_puts("*****Demo Fixed size allocator*****\n");
    unsigned long allocator_id = allocator_register(47);
    unsigned long addr1 = allocator_alloc(allocator_id);
    unsigned long addr2 = allocator_alloc(allocator_id);
    uart_puts("## Fixed size allocated memory: ");
    uart_send_ulong(addr1); uart_send(' ');
    uart_send_ulong(addr2), uart_send('\n');
    allocator_free(allocator_id, addr1);
    allocator_free(allocator_id, addr2);
    allocator_unregister(allocator_id);
    uart_puts("***********************************\n");

    uart_puts("*****Demo Varied-size allocator*****\n");
    unsigned long addrd = dynamic_alloc(5000);
    show_buddy_system();
    dynamic_free(addrd);
    show_buddy_system();
    uart_puts("************************************\n");
}

void vfs_test() {
    struct file *a = vfs_open("hello0", 0);
    uart_send_ulong((unsigned long)a);
    uart_send('\n');
    a = vfs_open("hello0", O_CREAT);
    uart_send_ulong((unsigned long)a);
    uart_send('\n');
    vfs_close(a);
    struct file *b = vfs_open("hello0", 0);
    uart_send_ulong((unsigned long)b);
    uart_send('\n');
    vfs_close(b);
}

void vfs_test_rw() {
    struct file* a = vfs_open("hello", O_CREAT);
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!\n", 7);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    char buf[64];
    memzero((unsigned long)buf, 64);
    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf);
}

void start_kernel() {
    uart_init();
    el1_vector_init();
    enable_core_timer();
    enable_irq();
    init_page_map();
    rootfs_init();

    uart_puts("Welcome to MiniKernel 0.0.7\n");

    vfs_test();
    vfs_test_rw();

    int ret;

    for (int i = 0; i < 2; ++ i) {
        ret = __clone((unsigned long)&foo, PF_KTHREAD, 0);
        if (ret == -1) {
            uart_puts("Error when creating privilege task ");
            uart_send_ulong(i);
            uart_send('\n');
        }
    }

    ret = __clone((unsigned long)&zombie_reaper, PF_KTHREAD, 0);
    ret = __clone((unsigned long)&create_user_process, PF_KTHREAD, 0);
    
    idle();
}
