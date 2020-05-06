#include "exception_handler.h"
#include "my_string.h"
#include "uart.h"
#include "kernel.h"

extern int reschedule;

void handler(){
    unsigned long long elr;
    unsigned long long esr;
    int ec;
    int iss;
    char res[30];

    asm volatile("mrs %0, ELR_EL1" : "=r"(elr));
    asm volatile("mrs %0, ESR_EL1" : "=r"(esr));

    unsign_itohexa(elr, res);
    uart_puts("Exception return address: 0x");
    uart_puts(res);
    uart_puts("\n");

    ec = (esr & 4227858432) >> 26;
    unsign_itohexa(ec , res);
    uart_puts("Exception class (EC): 0x");
    uart_puts(res);
    uart_puts("\n");

    iss = (esr & 33554431);
    unsign_itohexa(iss , res);
    uart_puts("Exception specfic syndrome (ISS): 0x");
    uart_puts(res);
    uart_puts("\n");

    if (ec == 21 && iss == 0){
        uart_puts("\n");
        uart_puts("Enable system timer.\n");

        asm(
            "mov x0, 1;"
            "msr cntp_ctl_el0, x0;"
            "mrs x0, cntfrq_el0;"
            "msr cntp_tval_el0, x0;"
            "mov x0, 2;"
            "ldr x1, =0x40000040;"
            "str x0, [x1];"
        );
        *CORE0_TIMER_IRQ_CTRL = 2;
    }
    return;
}

void el1_irq_isr(){
    struct task_struct* now = get_current();
    static int jiffies=0;
    char res[10];

    jiffies++;
    unsign_itoa(jiffies, res);
    uart_puts("In EL1, Core timer interrupt, jiffies ");
    uart_puts(res);
    uart_puts("\n");
    asm(
        "mrs x0, cntfrq_el0;"
        "msr cntp_tval_el0, x0;"
    );

    now->t_quantum++;
    if (now->t_quantum == 3){
        reschedule = 1;
        now->t_quantum = 0;
    }
    return;
}

void el0_irq_isr(){
    static int jiffies=0;
    char res[10];

    jiffies++;
    unsign_itoa(jiffies, res);
    uart_puts("In EL0, Core timer interrupt, jiffies ");
    uart_puts(res);
    uart_puts("\n");
    asm(
        "mrs x0, cntfrq_el0;"
        "msr cntp_tval_el0, x0;"
    );

    return;
}
