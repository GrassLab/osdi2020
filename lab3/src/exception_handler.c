#include "exception_handler.h"
#include "my_string.h"
#include "uart.h"

void handler(){
    unsigned long long elr;
    unsigned long long esr;
    char res[30];

    asm volatile("mrs %0, ELR_EL2" : "=r"(elr));
    asm volatile("mrs %0, ESR_EL2" : "=r"(esr));

    unsign_itohexa(elr, res);
    uart_puts("Exception return address: 0x");
    uart_puts(res);
    uart_puts("\n");

    unsign_itohexa( (esr & 4227858432) >> 26, res);
    uart_puts("Exception class (EC): 0x");
    uart_puts(res);
    uart_puts("\n");

    unsign_itohexa( (esr & 33554431), res);
    uart_puts("Exception specfic syndrome (ISS): 0x");
    uart_puts(res);
    uart_puts("\n");

    /*unsigned long long sp;
    asm volatile("mrs %0, SP_EL2" : "=r"(sp));
    unsign_itohexa(sp, res);
    uart_puts("SP_EL2: 0x");
    uart_puts(res);
    uart_puts("\n");*/

    return;
}
