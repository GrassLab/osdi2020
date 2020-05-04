#include "utils.h"
#include "mini_uart.h"
#include "entry.h"
#include "timer.h"
#include "sys.h"
#include "fork.h"
#include "irq.h"
#include "mm.h"
#include "peripherals/irq.h"
#include "peripherals/mini_uart.h"

unsigned int read_cntfrq(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

void write_cntp_tval(unsigned int val)
{
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}

unsigned int read_cntp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}

void handle_sync(unsigned long esr, unsigned long address)
{
    uart_send_hex(address);
    uart_send_hex(esr>>26);
    uart_send_hex(esr & 0xfff);
    return;
}

void handle_el0_sync(unsigned long par1, unsigned long par2)
{
    unsigned int val;
    asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_CLONE) {
        //uart_send("return from child\r\n");
        return privilege_task_create(0, 0, 0, 0);
    }
    if (val == SYS_EXIT_NUMBER) {
        for (int i = 0 ; i < NR_TASKS ; i++) {
            if (task[i] == current) {
                task[i]->state = par1;
                nr_tasks -= 1;
                break;
            }
        }
        //TODO free user page
        schedule();
    }
    if (val == SYS_UART_READ) {
        // par1 is buffer address
        // par2 is read size
        char c;
        int  i = 0;
        char *buf_ptr = par1;
        while(1) {
		    if(get32(AUX_MU_LSR_REG)&0x01) {
                c = get32(AUX_MU_IO_REG)&0xFF;
                *(buf_ptr + i) = c;
                i++;
                if (i == par2) {
                    break;
                }
            } 
            else if(i > 0) {
                break;
            }
	    }
        return i;
    }
    if (val == SYS_UART_WRITE) {
        unsigned long i;
        char *buf_ptr = par1; 
        for (i = 0 ; i < par2 ;i++) {
            uart_send(*(buf_ptr + i));
        }
        return i;
    }
    if (val == SYS_EXEC) {
        return do_exec(par1);
    }
    if (val == SYS_FORK) {
        return privilege_task_create(0, 0, 0, 0);
    }
    if (val == SYS_ID) {
        unsigned long i = current->task_id;
        return i;
    }
    if (val == SYS_KILL) {
        unsigned long pid = par1;
        struct task_struct *killed_child; 
        killed_child = (struct task_struct *)(LOW_KERNEL_STACK + (pid - 1) *PAGE_SIZE);
        killed_child->kill_flag = 1;
    }
    if (val == SYS_UART_EN) {
        put32(ENABLE_IRQS_1, AUX_IRQ);
    }
    return 0;
}

void handle_el1_sync(unsigned long esr, unsigned long address)
{
    unsigned int val;
	asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_TIME_IRQ) {
        sys_timer_init();
        enable_timer_controller();  // function in irq.c
        unsigned int cntfrq;
        unsigned int val;
        cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
        val = read_cntp_tval();
        core_timer_enable();
        enable_irq();
    }
    return;
}