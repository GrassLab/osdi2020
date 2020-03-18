#include "uart.h"
#include "gpio.h"
//#define PM_PASSWORD 0x5a000000
//#define PM_RSTC (volatile unsigned int*)(0x3F10001c)
//#define PM_WDOG (volatile unsigned int*)(0x3F100024)
//#define PM_RSTS ((volatile unsigned int*)(0x3F100020))
#define COMC 4

#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

struct hash{
    void (*function)();
    char *str;
//    struct hash *next;
};

struct hash commands[COMC];

void wait_command() {
    char command_buffer[50] = "";
    char byte;
    int buffer_counter = 0;
    uart_puts("# ");
    while(1) {
        command_buffer[buffer_counter++] = uart_getc();
        uart_send(command_buffer[buffer_counter - 1]);
        if(buffer_counter > 49) {
            buffer_counter = 0;
            for(int i = 0 ; i < buffer_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("Reach the limit of text buffer, reflush\n");
            uart_puts("# ");
        } else if(command_buffer[buffer_counter - 1] == '\n') {
            command_buffer[buffer_counter - 1] = '\0';
            buffer_counter = 0;
            command(command_buffer);
            for(int i = 0 ; i < buffer_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("\n# ");
        }
    }
}

void command(char *s) {
    for(int i = 0 ; i < COMC ; i++) {
        if(strcmp(s, commands[i].str) == 0) {
            commands[i].function();
            return;
        }
    }
    uart_puts("Unknown command: ");
    uart_puts(s);
}

void help() {
    uart_puts("The list of commands:\n");
    for(int i = 0 ; i < COMC ; i++) {
        uart_puts(commands[i].str);
        uart_send('\n');
    }
}

void hello() {
    uart_puts("Hello World!!");
}

void timestamp() {
    char quo_buf[30] = "";
    char re_buf[30] = "";
    char swp[30] = "";
    register unsigned long frq asm("x10");
    register unsigned long pct asm("x11");
    asm volatile(
        "mrs x10, cntfrq_el0 \n"
        "mrs x11, cntpct_el0 \n"
//        "udiv x2, x0, x1 \n"
//        "msub x3, x2, x1, x0 \n"
    );
    unsigned long re = 0;
    unsigned long quo = 0;
    quo = pct/frq;
    re = pct%frq;
    re = re * 1000 / frq;
    int idx = 0;
    while(idx < 3) {
        re_buf[2 - idx] = re%10 + 48;
        re /= 10;
        idx++;
    }
    idx = 0;
    while(quo) {
        quo_buf[idx++] = quo%10 + 48;
        quo /= 10;
    }
    for(int i = 0 ; i < idx ; i++) {
        swp[idx - i - 1] = quo_buf[idx - 1];
    }
    uart_puts(swp);
    uart_send('.');
    uart_puts(re_buf);
}

void reboot() {
    uart_puts("it will reboot.");
    reset();
}

void sys_init() {
    commands[0].str = "help\0";
    commands[0].function = &help;
    commands[1].str = "hello\0";
    commands[1].function = &hello;
    commands[2].str = "timestamp\0";
    commands[2].function = &timestamp;
    commands[3].str = "reboot\0";
    commands[3].function = &reboot;
}

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2)) {
        s1++, s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

//void reset(int tick){ // reboot after watchdog timer expire
//  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
//  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
//}

void reset()
{
    unsigned int r;
    // trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;   // boot from partition 0
    *PM_WDOG = PM_WDOG_MAGIC | 1000;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

//void cancel_reset() {
//  *PM_RSTC = PM_PASSWORD | 0; // full reset
//  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
//}