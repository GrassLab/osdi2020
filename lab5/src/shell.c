#include "mbox.h"
#include "branch.h"
#include "uart.h"
#include "irq.h"
#include "utils.h"

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }
        if (*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
}

void memset(char *str, char c, int n) {
    for (int i = 0; i < n; i++) {
        str[i] = c;
    }
}

void Help() {
    uart_puts("exc: svc!\n");
    uart_puts("irq: interrupt!\n");
    uart_puts("help: help!\n");
}


void shell(void)
{
    
    uart_puts("SHELL IS START!!\n");
    uart_puts("# ");
//  el0 cannot get_el 
/*  
    unsigned long current_el;
    current_el = get_el();
    uart_puts("Current EL: ");
    uart_hex(current_el);
    uart_puts("\n");
*/
    char str[200];
    memset(str, '\0', 200);
    int i = 0;
    while (1) {
        char c = uart_getc();
        str[i] = c;
        uart_send(c);
        if (c == '\n') {
            if (strcmp(str, "exc\n") == 0) {
                asm volatile("svc #1");
            } else if (strcmp(str, "irq\n") == 0) {
				asm volatile("mov x0, #0x0");
        		asm volatile("svc #0x80");
            } else if (strcmp(str, "help\n") == 0) {
                Help();
            } else {
                uart_puts("Err: ");
                str[i] = '\0';
                uart_puts(str);
                uart_puts(" command not found, try <help>\n");
            }
            i = 0;
            memset(str, '\0', 200);
            uart_puts("# ");
        } else {
            i++;
        }
    }
}
