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

#define INPUT_BUFFER_SIZE 64

void system_start()
{
    uart_print("-------------------------\n");
    uart_print("Raspberry Pi 3B+ is start\n");
    uart_print("-------------------------\n");
    uart_print("Author  : Hsu, Po-Chun\n");
    uart_print("Version : 5.3.4\n");
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

    uart_send('\n');
    // check user space set in 0xffff000000000000
    uart_send_hex(((unsigned long *)(0xffff000000000000))[0]);
    uart_send('\n');

    uart_send('=');

    while (1)
    {
        if (check_reschedule())
            schedule();
    }
    uart_send('?');
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
    while (1)
    {
        if (check_reschedule())
        {
            schedule();
        }
    }
}

int kernel_main()
{

    char *s = "Writing through MMIO mapped in higher half!\r\n";

    // set uart
    uart_init();
    init_printf(0, putc);

    int x = 0;
    uart_send_hex((unsigned int)(unsigned long)&x);
    uart_send('\n');

    mmu_init();

    //run_program();

    uart_send_hex((unsigned int)(unsigned long)&x);
    uart_send('\n');

    signal_init();

    system_start();

    /* You can't know Current EL, if you in EL0 */
    /*
    uart_puts("Current EL: ");
    uart_send_int(get_current_el());
    uart_puts("\n");
    */

    /*
    unsigned long t = virtual_to_physical(0xffff000000000000);
    t = virtual_to_physical(0xffffffff00030010);
    uart_send_hex(t);
    */

    task_init();
    /*
    privilege_task_create((unsigned long)task_111, 0);
    */
    privilege_task_create((unsigned long)task_1, 0);
    privilege_task_create((unsigned long)task_11, 0);
    // privilege_task_create((unsigned long)task_2, 0); // fork: delay, shell
    privilege_task_create((unsigned long)task_3, 0);
    privilege_task_create((unsigned long)task_program, 0);
    //privilege_task_create((unsigned long)task_4, 0);
    // privilege_task_create((unsigned long)user_test, 0);

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
