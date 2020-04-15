#include "uart.h"
#include "gpio.h"
#include "foo.h"

#define bufsize 255

extern char _end[];
extern char _begin[];

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
    uart_puts("  dmidecode\n\tShow hardware info\n");
    uart_puts("  loadimg\n\tLoadimg from PC [Only support pc.py]\n");
    uart_puts("  exc\n\tException test\n");
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

int read_int() {
    int val = 0;
    while (1) {
        char ch = uart_getc();
        if (ch == '\n')
            break;
        val *= 10;
        val += (ch - '0');
    }
    return val;
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

#define chunksize 128

void load_and_jump() {
    uart_puts("ok");
    char* addr = (char*)0x79999;
    int kernel_size = read_int();
    print_int(kernel_size);
    uart_puts("\n");
    int num_chunk = kernel_size / chunksize;
    int last_chunk = kernel_size - num_chunk * chunksize;
    char byte;

    for (int c = 0; c <= num_chunk; ++ c) {
        int size_this_chunk = (c == num_chunk) ? last_chunk : chunksize;
        print_int(size_this_chunk);
        uart_puts("...ready");
        for (int i = 0; i < size_this_chunk; ++ i) {
            byte = uart_getc();
            addr[c * chunksize + i] = byte;
        }
        print_int(c);
        uart_puts("done");
    }
    
    uart_puts("okload");
    for (int i = 0; i < 100000; ++ i) {
    }
    b8((unsigned long int*)0x80000);
}

void copy_self() {
    char *begin = _begin;
    char *end = _end;
    char *tmpaddr = (char*)0x100000;
    while (begin <= end) {
        *tmpaddr = *begin;
        tmpaddr ++;
        begin ++;
    }
    void (*func_ptr)() = load_and_jump;
    unsigned long int func_pos = (unsigned long int)func_ptr;
    void (*tmp_func)() = (void(*)())(func_pos - (unsigned long int)_begin + 0x100000);
    tmp_func();
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
        } else if (strcmp(cmdbuf, "dmidecode") == -1) {
            show_board_revision();
            show_vc_memory();
        } else if (strcmp(cmdbuf, "loadimg") == -1) {
            copy_self();
        } else if (strcmp(cmdbuf, "exc") == -1) {
            unsigned int a = 0x9487;
            asm("mov x2, %0" : "=r"(a));
            asm("svc #1");
            asm("mov %0, x2" : "=r"(a));
            uart_puts("x2: ");
            uart_hex(a);
            uart_puts("\n");
        } else if (strcmp(cmdbuf, "irq") == -1) {
            asm("svc #2");
            // core_timer_enable();
        } else if (strcmp(cmdbuf, "getel") == -1) {
            uart_puts("EL: ");
            print_int(get_el());
            uart_send('\n');
        } else {
            unsupported(cmdbuf);
        }
    }
}
