#include "uart.h"
#include "gpio.h"

#define bufsize 255

void read_cmd(char* cmdbuf) {
    for (int i = 0; i < bufsize; ++ i) {
        cmdbuf[i] = 0;
    }
    int cur_pos = 0;
    while (1) {
        char c = uart_getc();
        if (c == '\n') {
            uart_send('\n');
            return;
        }
        // backspace
        if (c == 127) {
            if (cur_pos > 0) {
                uart_puts("\b \b");
                cmdbuf[--cur_pos] = 0;
            }
        } else {
            if (cur_pos < 254) {
                uart_send(c);
                cmdbuf[cur_pos++] = c;
            }
        }
    }
}

void hello() {
    uart_puts("Hello World!\n");
}

void help() {
    uart_puts("Usage:  <command>\n");
    uart_puts("Supported commands are:\n");
    uart_puts("  help\n\tShow this help message\n");
    uart_puts("  hello\n\tHello world\n");
    uart_puts("  reboot\n\tReboot the rbpi3\n");
    uart_puts("  uptime\n\tTell how long the system has been running\n");
    uart_puts("  timestamp\n\tAlias of uptime\n");
    // uart_puts("Use 'help' for getting help for above commands.\n");
}

int strcmp(char* a, char* b) {
    int cur = 0;
    while ((a[cur] != 0) || (b[cur] != 0)) {
        if (a[cur] != b[cur]) {
            return cur;
        }
        cur ++;
    }
    return -1;
}

void unsupported(char* s) {
    if (s[0] == '\0') return;
    uart_puts("\'");
    uart_puts(s);
    uart_puts("\' is not recognized as an internal or external command, operable program or batch file\n");
}

#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

void reset() { 
    uart_puts("Rebooting...\n");
    unsigned int r;
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;  
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

void print_int(int number) {
    char tbuf[bufsize];
    for (int i = 0; i < bufsize; ++ i) {
        tbuf[i] = 0;
    }
    int cur = bufsize - 2;
    while (number > 0 && cur > 0) {
        tbuf[cur] = (number % 10) + '0';
        number = number / 10;
        cur --;
    }    
    uart_puts(tbuf + cur + 1);
}

void get_time() {
    register unsigned long f, t;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));

    double time = 1. * t / f;
    print_int((int)time);
    uart_puts(".");
    print_int((int)((time - (int)time) * 1000000));
    uart_puts("\n");
}

void main() {
    uart_init();

    char cmdbuf[bufsize];
    uart_puts("\n");
    uart_puts("Welcome\n");
    uart_getc();
    
    while(1) {
        uart_puts("# ");
        read_cmd(cmdbuf);
        if (strcmp(cmdbuf, "help") == -1) {
            help();
        } else if (strcmp(cmdbuf, "hello") == -1) {
            hello();
        } else if (strcmp(cmdbuf, "reboot") == -1) {
            reset();
        } else if (strcmp(cmdbuf, "timestamp") == -1) {
            get_time();
        } else if (strcmp(cmdbuf, "uptime") == -1) {
            get_time();
        } else {
            unsupported(cmdbuf);
        }
    }
}
