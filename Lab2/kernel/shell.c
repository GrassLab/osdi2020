#include "uart.h"
#include "gpio.h"
#include "shell.h"
#include "mbox.h"
//#define PM_PASSWORD 0x5a000000
//#define PM_RSTC (volatile unsigned int*)(0x3F10001c)
//#define PM_WDOG (volatile unsigned int*)(0x3F100024)
//#define PM_RSTS ((volatile unsigned int*)(0x3F100020))

#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

/*
struct hash commands[] =  {{hello, "hello"},
                                {timestamp, "timestamp"},
                                {reboot, "reboot"},
                                {help, "help"},
                                {mboxinfo, "mboxinfo"}};
*/

void wait_command() {
    struct hash commands[] =  {{hello, "hello"},
                                {timestamp, "timestamp"},
                                {reboot, "reboot"},
                                {help, "help"},
                                {mboxinfo, "mboxinfo"}};
    char command_buffer[50] = "";
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
            command(command_buffer, commands);
            for(int i = 0 ; i < buffer_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("\n# ");
        }
    }
}

void command(char *s, struct hash *commands) {
    int commands_size = sizeof(commands) / sizeof(commands[0]);
    commands_size = 5;
    int command_len;
    char tmp[64] = "";
    for(int i = 0 ; i < commands_size ; i++) {
        command_len = length(commands[i].str);
        strcpy(s, tmp);
        tmp[command_len] = '\0';
        if(strcmp(tmp, commands[i].str) == 0) {
            commands[i].function(tmp + command_len + 1);
            return;
        }
    }
    uart_puts("\rUnknown command: ");
    uart_puts(s);
}

void help(const char *args) {
    /*
    //int commands_size = sizeof(commands) / sizeof(commands[0]);
    int commands_size = 5;
    uart_puts("The list of commands:\n");
    for(int i = 0 ; i < commands_size ; i++) {
        uart_puts(commands[i].str);
        uart_puts("\r\n");
    }
    */
    uart_puts("sorry, it's broken.\n");
}

void hello(const char *args) {
    uart_puts("\n\rHello World!!");
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
    uart_puts("it will reboot.");
    reset();
}


//void reset(int tick){ // reboot after watchdog timer expire
//  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
//  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
//}

void mboxinfo(const char *args) {
    uart_puts("Mailbox info:\n");
    unsigned int rtag;
    if(strcmp(args, "VC") == 0) {
        rtag = MBOX_TAG_VC_MEMORY;
    } else if (strcmp(args, "BREV") == 0) {
        rtag = MBOX_TAG_BREVISION;
    } else if(strcmp(args, "SER") == 0) {
        rtag = MBOX_TAG_GETSERIAL;
    } else {
        uart_puts("Unknown mbox query.\n");
        return;
    }
    // get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = rtag;
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        if(strcmp(args, "SER") == 0) {
            uart_puts("The serial number is: ");
            uart_hex(mbox[6]);
            uart_hex(mbox[5]);
            uart_puts("\n");
        } else if(strcmp(args, "BREV") == 0) {
            uart_puts("The board revision is: ");
            uart_hex(mbox[5]);
        } else if(strcmp(args, "VC") == 0) {
            uart_puts("The VC memory is: \n");
            uart_puts("Base: 0x");
            uart_hex(mbox[6]);
            uart_puts("\nSize: 0x");
            uart_hex(mbox[5]);
            uart_puts("\n");
        }
    } else {
        uart_puts("Unable to query!\n");
    }
}

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


static inline void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

static inline void reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
}

int length(const char *s) {
    int tmp = 0;
    while(*s++) { tmp++; };
    return tmp;
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