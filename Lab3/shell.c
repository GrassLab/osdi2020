#include "uart.h"
#include "gpio.h"
#include "shell.h"
#include "img_loader.h"
//#define PM_PASSWORD 0x5a000000
//#define PM_RSTC (volatile unsigned int*)(0x3F10001c)
//#define PM_WDOG (volatile unsigned int*)(0x3F100024)
//#define PM_RSTS ((volatile unsigned int*)(0x3F100020))
#ifndef REBOOT_PM
#define REBOOT_PM
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020
#endif
/*
struct hash{
    void (*function)(void);
    char *str;
//    struct hash *next;
};


*/

//struct hash commands[COMC];
static inline void offset(char *s, int start, int end) {
    while(start < end) {
        s[end] = s[end + 1];
        end--;
    }
}

const struct hash commands[] =  {{loadimg, "loadimg"},
                                {timestamp, "timestamp"},
                                {reboot, "reboot"}};

void wait_command() {
    char input_c;
    char command_buffer[64] = "";
    int buffer_counter = 0;
    int buffer_point = 0;
    uart_puts("# ");
    while(1) {
        input_c = uart_getc();
        if(input_c == 127) {
            if(buffer_counter == 0) {
                continue;
            }
            buffer_counter--;
            buffer_point--;
            uart_puts("\b \b");
            continue;
        } 

        if(buffer_point == buffer_counter) {
            uart_send(input_c);
            buffer_counter++;
            command_buffer[buffer_point++] = input_c;
        } else {
            for(int i = buffer_point ; i > 0 ; i--) {
                uart_send('\b');
            }
            offset(command_buffer, buffer_point, buffer_counter);
            command_buffer[buffer_point++] = input_c;
            uart_puts(command_buffer);
        }
        if(buffer_counter > 49) {
            buffer_counter = 0;
            buffer_point = 0;
            for(int i = 0 ; i < buffer_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("\n\rReach the limit of text buffer, reflush\n");
            uart_puts("# ");
        } else if(command_buffer[buffer_counter - 1] == '\n') {
            command_buffer[buffer_counter - 1] = '\0';
            buffer_counter = 0;
            buffer_point = 0;
            command(command_buffer);
            for(int i = 0 ; i < buffer_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("\n# ");
        }
    }
}

void command(char *s) {
    int commands_size = sizeof(commands) / sizeof(commands[0]);
    int command_len;
    char tmp[64] = "";
    for(int i = 0 ; i < commands_size ; i++) {
        command_len = length(commands[i].str);
        strcpy(s, tmp);
        if(tmp[command_len] == ' ') {
            tmp[command_len] = '\0';
        }
        if(strcmp(tmp, commands[i].str) == 0) {
            commands[i].function(tmp + command_len + 1);
            return;
        }
    }
    
    uart_puts("\rUnknown command: ");
    uart_puts(s);
}




void timestamp(const char *args) {
    if(length(args) != 0) {
        uart_puts("\rtimestamp error : it doesn't accept flags.");
        return;
    }
    char quo_buf[30] = "";
    char re_buf[30] = "";

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
    re = (re * 1000) / frq;

    num_string(re, re_buf, 10);
    num_string(quo, quo_buf, 10);

    uart_puts("\n\r");
    uart_send('[');
    uart_puts(quo_buf);
    uart_send('.');
    uart_puts(re_buf);
    uart_send(']');
}

void reboot(const char *args) {
    if(length(args) != 0) {
        uart_puts("\rreboot error: it doesn't accept flags.");
        return;
    }
    uart_send('\r');
    uart_puts("it will reboot.");
    reset();
}
/*
void sys_init() {
    commands[0].str = "help\0";
    commands[0].function = help;
    commands[1].str = "hello\0";
    commands[1].function = hello;
    commands[2].str = "timestamp\0";
    commands[2].function = timestamp;
    commands[3].str = "reboot\0";
    commands[3].function = reboot;
}
*/

void loadimg(const char *args) {
    char tmp[64];
    strcpy(args, tmp);
    tmp[2] = '\0';
    int base;
    if(strcmp("0x", tmp) == 0) {
        strcpy(args + 2, tmp);
        base = 16;
    } else {
        strcpy(args, tmp);
        base = 10;
    }
    if(!is_num_string(tmp)) {
        uart_puts("loadimg error : need pure and positive number string.\n");
        return;
    }
    int address = string_num(tmp, base) + 0x80000;
    num_string(address, tmp, 16);

    timestamp("");
    uart_puts("\r\nNow loading kernel image start at: 0x");
    uart_puts(tmp);
    uart_puts("\n");
    load_kernel(address);
    return;
}


int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++, s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void strcpy(const char* s1, char* s2) {
    while(*s1) {
        *s2 = *s1;
        s2++, s1++;
    }
    *s2 = '\0';
}

int length(const char *s) {
    int tmp = 0;
    while(*s++) { tmp++; };
    return tmp;
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



static inline void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

static inline void reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
}

int is_num_string(const char* s) {
    while(*s) {
        if(!( (*s <=  57 && *s >= 48) || (*s <= 90 && *s >= 65) || (*s <= 122 && *s >= 61) )) {
            return 0;
        }
        s++;
    }
    return 1;
}

int string_num(const char *s, int base) {
    int num = 0;
    char c;
    while( (c = *s++) ) {
        if(base > 10 && !(c >= 48 && c <= 57)) {
            if(c < 97) {
                num *= base;
                num += (c + 10 - 65);
            } else {
                num *= base;
                num += (c + 10 - 97);
            }
        } else {
            num *= base;
            num += (c - 48);
        }
    }
    return num;
}

void num_string(long long num, char *s, int base) {
    long long ab;
    if(num < 0) {
        ab = -num;
    } else {
        ab = num;
    }
    int i = 0;
    while(ab) {
        int tmp = ab % base;
        if(tmp >= 10) {
            s[i++] = tmp - 10 + 65;
        } else {
            s[i++] = tmp + 48;
        }
        ab /= base;
    }
    if( i == 0 ){
        s[i++] = '0';
    }
    if(num < 0 && base == 10) {
        s[i++] = '-';
    }
    reverse(s, 0, i - 1);
    s[i++] = '\0';
}