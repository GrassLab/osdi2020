#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "mailbox.h"
#define S_MAX 100

void read_string(char *buf){
    int i=0;
    char single;   // Important that this in an int to distinguish EOF from input.

    while((single = uart_getc()) != '\n'){
        if(single == '\177'){
            if(i>=1){
                uart_puts("\b \b");
                i--;
            }
        }else if(i >= (S_MAX-1)){
            ;           // Too many, do not save or maybe indicate error
        }else{
            uart_send(single);
            buf[i++] = single;
        }
    }
    buf[i] = '\0';  // Add termination
    printf("\n");
}

int strcmp(char *s1, char *s2){
    int i;
    for (i = 0; s1[i] == s2[i]; i++){
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

unsigned long get_cpu_time(){
    unsigned long t;
    asm volatile("mrs %0, cntpct_el0" : "=r"(t)); // get cpu count
    return t;
}

unsigned long get_cpu_clock_frequence(){
    unsigned long f;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    return f;
}

void timestamp(){
    unsigned long t = get_cpu_time();
    unsigned long f = get_cpu_clock_frequence();
    unsigned long x = t/f;
    unsigned long y = (t*10000000)/f-(x*10000000);

    printf("[%d.%d]\n", x, y);
}

#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

// reboot
void reset()
{
    unsigned int r;
    // trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;   // boot from partition 0
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

void main(){
    // set up serial console
    uart_init();
    
    // get the board's unique serial number with a mailbox call
    unsigned int mailbox[8];
    mailbox[0] = 8*4;                  // length of the message
    mailbox[1] = MAILBOX_REQUEST;         // this is a request message
    
    mailbox[2] = MAILBOX_TAG_GETSERIAL;   // get serial number command
    mailbox[3] = 8;                    // buffer size
    mailbox[4] = 8;
    mailbox[5] = 0;                    // clear output buffer
    mailbox[6] = 0;

    mailbox[7] = MAILBOX_TAG_LAST;

    // uart_puts("Hello World! ttt\n");

    // send the message to the GPU and receive answer
    if (mailbox_call(mailbox, MAILBOX_CH_PROP)) {
        uart_puts("My serial number is: ");
        printf("0x%x%x\n", mailbox[5], mailbox[6]);
    } else {
        uart_puts("Unable to query serial!\n");
    }

    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
        char str[S_MAX] = {0};

        uart_puts("# ");
        read_string(str);

        if(strcmp(str,"")==0){
            ;
        }else if(strcmp(str,"hello")==0){
            printf("helloooo\n");
        }else if(strcmp(str,"timestamp")==0){
            timestamp();
        }else if(strcmp(str,"reboot")==0){
            reset();
            printf("reboot\n");
        }else if(strcmp(str,"help")==0){
            printf("%s%s%s%s",
                "hello: print Hello World!\n",
                "help: help\n",
                "loadimg: load kernel img info by mailbox\n",
                "reboot: reboot rpi3\n",
                "timestamp: get current timestamp\n");
        }else if(strcmp(str,"loadimg")==0){
            get_board_revision();
            get_VC_memory();
        }else{
            printf("Err: command %s not found, try <help>\n", str);
        }
    }
}
