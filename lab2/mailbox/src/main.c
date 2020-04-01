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
            get_VC_memory();
            get_board_revision();
        }else{
            printf("Err: command %s not found, try <help>\n", str);
        }
    }
}
