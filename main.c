#include "uart.h"
#include "printf.h"
#include "pcsh.h"
#include "screen.h"
#include "string.h"

#include "syscall.h"
#include "irq.h"
#include "bottom_half.h"

#include "exception.h"
#include "task.h"

#include "signal.h"

#include "mm.h"
#include "mm_allocator.h"

#include "vfs.h"
#include "tmpfs.h"

#include "fat32.h"

#define INPUT_BUFFER_SIZE 64

void system_start()
{
    uart_print("-------------------------\n");
    uart_print("Raspberry Pi 3B+ is start\n");
    uart_print("-------------------------\n");
    uart_print("Author  : Hsu, Po-Chun\n");
    uart_print("Version : 8.0.0\n");
    uart_print("-------------------------\n");
    get_board_revision();
    get_vc_memory();
    /*
    get_frame_buffer();
    showpicture();
    */
}

void delay_c(char c)
{
    for (int i = 0; i < 10000; i++)
    {
        uart_send(c);
        // very very very slow in real rpi3, but very very very fast in qemu
        asm volatile(
            "mov  x0, #0xfffff\n"
            "loop_delay_c_0: subs  x0, x0, #1\n"
            "bne   loop_delay_c_0\n");
    }
}

void delay(unsigned long num)
{
    for (unsigned long i = 0; i < num; i++)
    {
        asm volatile("nop");
    }
}

/* User task */
void user_task_3()
{
    // delay_c, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay_c('3');
}

void user_task_4()
{
    // delay_c, depend on cpu frequency, so in rpi3 B+ and qemu is different
    delay_c('4');
}

void user_syscall_test()
{
    printf("\ntask_id: %d\n", get_taskid());
    int pid = fork();
    //int pid = 0;
    if (pid == 0)
    {
        printf("\nPID: %d\n\r", pid);
        exec((unsigned long)user_task_4);
    }
    else
    {
        printf("\nPID: %d\n\r", pid);
        exec((unsigned long)pcsh);
        exit(0);
    }
}

/* Kernel task */
void task_1()
{
    printf("\ntask_id: %d\n", get_taskid());
    delay_c('1');
}

void task_11()
{
    printf("\ntask_id: %d\n", get_taskid());
    delay_c('_');
}

void task_111()
{
    printf("\ntask_id: %d\n", get_taskid());
    delay_c('?');
}

void task_2()
{
    printf("\ntask_id: %d\n", get_taskid());
    do_exec((unsigned long)user_syscall_test);
    uart_send('^');
    while (check_reschedule())
        schedule();
}

void task_3()
{
    printf("\ntask_id: %d\n", get_taskid());
    uart_send_hex((unsigned long)user_task_3);

    //((void (*)(void))user_task_3)();
    do_exec((unsigned long)user_task_3);

    /*
    uart_send('\n');
    // check user space set in 0xffff000000000000
    uart_send_hex(((unsigned long *)(0xffff000000000000))[0]);
    uart_send('\n');

    uart_send('=');
    */

    if (check_reschedule())
        schedule();
}

void task_4()
{
    while (1)
    {
        bottom_half_router();
        if (check_reschedule())
            schedule();
    }
}

void foo()
{
    int tmp = 5;
    printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
    exit(0);
}

void test()
{
    // int cnt = 1;
    register int cnt = 1;
    if (fork() == 0)
    {
        fork();
        delay(10000);
        fork();
        while (cnt < 10)
        {
            // printf("Task id: %d, cnt: %d address: %x\n\r", get_taskid(), cnt, &cnt);
            printf("Task id: %d, cnt: %d \n\r", get_taskid(), cnt);
            delay(1000000);
            ++cnt;
        }
        exit(0);
        printf("Should not be printed\n\r");
    }
    else
    {
        // printf("Task %d before exec, cnt address 0x%x, cnt value %d\n\r", get_taskid(), &cnt, cnt);
        printf("Task %d before exec, cnt value %d\n\r", get_taskid(), cnt);
        //kill(1, SIGKILL);
        exec(foo);
    }
}
void user_test()
{
    do_exec((unsigned long)test);
}

void run_program()
{
    extern unsigned long _binary_test_img_start;
    unsigned long program_start = (unsigned long)&_binary_test_img_start;

    ((void (*)(void))program_start)();
}

void task_program()
{
    extern unsigned long _binary_test_img_start;
    extern unsigned long _binary_test_img_size;
    unsigned long program_start = (unsigned long)&_binary_test_img_start;
    unsigned long program_size = (unsigned long)&_binary_test_img_size;

    printf("\ntask_id: %d\n", get_taskid());

    _do_exec(program_start, program_size);
    //do_exec((unsigned long)test);
    if (check_reschedule())
    {
        schedule();
    }
}
void task_program_2()
{
    extern unsigned long _binary_test2_img_start;
    extern unsigned long _binary_test2_img_size;
    unsigned long program_start = (unsigned long)&_binary_test2_img_start;
    unsigned long program_size = (unsigned long)&_binary_test2_img_size;

    printf("\ntask_id: %d\n", get_taskid());

    _do_exec(program_start, program_size);
    //do_exec((unsigned long)test);
    if (check_reschedule())
    {
        schedule();
    }
}

void test_memory_allocation()
{
    init_buddy_system();

    unsigned long m1 = get_free_space(2048);
    printf("get memory address: %x\n", m1);

    unsigned long m2 = get_free_space(PAGE_SIZE * 256);
    printf("get memory address: %x\n", m2);

    unsigned long m3 = get_free_space(4096);
    printf("get memory address: %x\n", m3);

    unsigned long m4 = get_free_space(2048);
    printf("get memory address: %x\n", m4);

    unsigned long m5 = get_free_space(4097);
    printf("get memory address: %x\n", m5);

    unsigned long m6 = get_free_space(PAGE_SIZE * 512);
    printf("get memory address: %x\n", m6);

    // this will fail
    unsigned long m7 = get_free_space(PAGE_SIZE * 1024);
    printf("get memory address: %x\n", m7);

    free_space(m3);
    free_space(m1);

    fixed_size_allocator_t o_allocator = register_fixed_size_allocator();
    unsigned long o1, o2, o3;
    o1 = (unsigned long)kmalloc_8(&o_allocator);
    free_fixed_memory(&o_allocator, o1);
    o1 = (unsigned long)kmalloc_8(&o_allocator);
    o2 = (unsigned long)kmalloc_8(&o_allocator);
    free_fixed_memory(&o_allocator, o1);
    free_fixed_memory(&o_allocator, o2);
    o1 = (unsigned long)kmalloc_256(&o_allocator);
    o2 = (unsigned long)kmalloc_256(&o_allocator);
    o3 = (unsigned long)kmalloc_32(&o_allocator);
    free_fixed_memory(&o_allocator, o1);
    free_fixed_memory(&o_allocator, o2);
    free_fixed_memory(&o_allocator, o3);

    varied_size_allocator_t v_allocator = register_varied_size_allocator();
    unsigned long v1, v2, v3;
    v1 = (unsigned long)kmalloc(&v_allocator, 32);
    v2 = (unsigned long)kmalloc(&v_allocator, 64);
    free_varied_memory(&v_allocator, v1);
    free_varied_memory(&v_allocator, v2);
    v1 = (unsigned long)kmalloc(&v_allocator, 32);
    v2 = (unsigned long)kmalloc(&v_allocator, 128);
    free_varied_memory(&v_allocator, v1);
    v3 = (unsigned long)kmalloc(&v_allocator, 128);
    free_varied_memory(&v_allocator, v2);
    free_varied_memory(&v_allocator, v3);
}

void lab7()
{
    printf("required 1, 2 test\n");
    filesystem_t fs = tmpfs_filesystem();
    register_filesystem(&fs);

    file_t *a = vfs_open("hello", O_OPEN);
    assert(a == NULL);
    a = vfs_open("hello", O_CREAT);
    assert(a != NULL);
    vfs_close(a);
    file_t *b = vfs_open("hello", O_OPEN);
    assert(b != NULL);
    vfs_close(b);

    printf("================================\n");
    printf("required 3 test\n");
    char buf[32];
    a = vfs_open("hello", O_OPEN);
    b = vfs_open("world", O_CREAT);

    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", O_OPEN);
    a = vfs_open("world", O_OPEN);

    int sz;
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("buf: %s\n", buf); // should be Hello World!

    printf("================================\n");
    printf("test finished\nhalt\n");
}

void lab8_test()
{
    fat32_init();
    printf("================================\n");
    printf("test finished\nhalt\n");
}

/* from lab6 to lab8, just in one thread without VM and interrupt */
int kernel_main()
{
    uart_init();
    init_printf(0, putc);
    system_start();

    lab8_test();

    while (1)
    {
    }
    return 0;
}

int kernel_main_full()
{

    // set uart
    uart_init();
    init_printf(0, putc);

    mmu_init();

    signal_init();

    system_start();

    task_init();

    privilege_task_create((unsigned long)task_1, 0);

    privilege_task_create((unsigned long)task_program, 0);
    privilege_task_create((unsigned long)task_program_2, 0);

    // enable_irq();

    // core timer enable, every 1ms
    core_timer(1);
    //_core_timer_enable();
    while (1)
    {
        /*
        schedule();
    */
    }

    return 0;
}
