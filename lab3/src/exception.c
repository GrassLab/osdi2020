#include "io.h"
#include "map.h"
#include "irq.h"
#include "info.h"
#include "time.h"
#include "timer.h"

extern char *exec_ptr;
extern unsigned int task_ptr;

const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1T",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};

void show_invalid_entry_message(int type,
        unsigned long esr,
        unsigned long elr) {
    if (type != -1)
        printf("Type: %s" NEWLINE, entry_error_messages[type]);
    printf("Exception return address 0x%x" NEWLINE
            "Exception class (EC) 0x%x" NEWLINE
            "Instruction specific syndrome (ISS) 0x%x" NEWLINE,
            elr , esr>>26, esr & 0xfff);
    return;
}

void syscall(unsigned int code, long x0, long x1, long x2, long x3,
        long x4, long x5) {
    //printf("syscall: %d" NEWLINE, code);
    switch (code) {
        case 0:
            sys_core_timer_enable();
            __asm__ volatile("mov x0, #0");
            break;
        case 1:
            sys_timestamp(); 
            __asm__ volatile("mov x0, #0");
            break;
        case 2:
            get_current_el();
            puts("interrupt disabled");
            //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
            __asm__ volatile ("msr daifclr, #0x2");
            __asm__ volatile("mov x0, #0");
            break;
        case 4:
            enable_irq();
            while(task_ptr && !exec_ptr){
#define record_elr
#ifdef record_elr
                unsigned long elr, nelr; 
                __asm__ volatile("mrs %0, elr_el1": "=r"(elr));
#endif

                void (*task)(void) = pop_deffered();
                //printf("task addr %x" NEWLINE, task);
                task();  

#ifdef record_elr
                __asm__ volatile("mrs %0, elr_el1": "=r"(nelr));
                //printf("%x vs %x" NEWLINE, elr, nelr);
                if(elr != nelr){
                    //puts(NEWLINE NEWLINE "BOTTOM DIFFERENCE!!" NEWLINE NEWLINE);
                    __asm__ volatile("msr elr_el1, %0":: "r"(elr));
                }
#endif 
            }
            __asm__ volatile("mov x0, #0");
            break;
        default:
            __asm__ volatile("mov x0, #1");
            break;
    }
    return;
}

void exception_handler(long x0, long x1, long x2, long x3, long x4, long x5) {
    unsigned long elr, esr, code, ret = 1;

    __asm__ volatile("mov %0, x8\n"
            "mrs %1, elr_el1\n"
            "mrs %2, esr_el1" :
            "=r"(code), "=r"(elr), "=r"(esr));

    unsigned int ec  = esr >> 26;
    unsigned int iss = esr & 0xfff;

    //get_current_el();

    switch (ec) {
        case 0x15:
            if (iss == 0) {
                //puts("===================");
                //printf("syscall code: %d" NEWLINE , code);

                syscall(code, x0, x1, x2, x3, x4, x5);
                __asm__ volatile("mov %0, x0" : "=r"(ret));
                /* the return value will stored in x0 register */
                //printf("syscall return value %d" NEWLINE, ret);
                //puts("===================");
                if (ret == 0) return;
                printf("syscall failed with code number: %d" NEWLINE, code);
            }
            break;
        default:
            break;
    }
    show_invalid_entry_message(-1, esr, elr);
}
