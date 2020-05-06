#include "exception_handler.h"
#include "my_string.h"
#include "uart.h"
#include "kernel.h"

extern int reschedule;

void excep_handler(unsigned long long sp){
//void excep_handler(){
    unsigned long long elr;
    unsigned long long esr;
    int ec;
    int iss;
    char res[30];

    asm volatile("mrs %0, ELR_EL1" : "=r"(elr));
    asm volatile("mrs %0, ESR_EL1" : "=r"(esr));

    ec = (esr & 4227858432) >> 26;
    iss = (esr & 33554431);

    if (ec == 21 && iss == 0){
        int syscall_num;

        char *s;
        char c;

        char **ptr;

        void (*func)();

        int child_id;
        struct task_struct* now;
        int *val;
        char res[10];

        asm volatile("mov %0, x8" : "=r"(syscall_num));
        switch (syscall_num){
            case 0:
                s = (char *)sp;
                c = uart_getc();
                *s = c;
                break;
            case 1:
                ptr = (char **)sp;
                uart_puts(*ptr);
                break;
            case 2:
                func = (void (*)())sp;
                do_exec(func);
                break;
            case 3:
                child_id = do_fork();
                struct task_struct* now = get_current();
                itoa(child_id, res);
                uart_puts("---------Current ID = ");
                uart_puts(res);
                uart_puts("\n");
                if (now->id == child_id){
                    val = (int *)sp;
                    *val = 0;
                }
                else {
                    val = (int *)sp;
                    *val = child_id;
                }
                break;
        }
        return;
    }

    unsign_itohexa(elr, res);
    uart_puts("Exception return address: 0x");
    uart_puts(res);
    uart_puts("\n");

    unsign_itohexa(ec , res);
    uart_puts("Exception class (EC): 0x");
    uart_puts(res);
    uart_puts("\n");

    unsign_itohexa(iss , res);
    uart_puts("Exception specfic syndrome (ISS): 0x");
    uart_puts(res);
    uart_puts("\n");

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
    struct task_struct* now = get_current();
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

    now->t_quantum++;
    if (now->t_quantum == 3){
        reschedule = 1;
        now->t_quantum = 0;
    }
    return;
}
