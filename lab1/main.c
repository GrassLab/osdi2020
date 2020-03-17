#include "uart.h"
#define MMIO_BASE       0x3F000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

void reset()
{
    unsigned int r;
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;  
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}
//https://github.com/torvalds/linux/blob/master/drivers/watchdog/bcm2835_wdt.c

unsigned long long get_system_frequency() 
{
    unsigned long long res = 0;
    __asm volatile
    (
        "MRS %[result], CNTFRQ_EL0": [result] "=r" (res)
    );
    return res;
}

unsigned long long get_system_count() 
{
    unsigned long long res = 0;
    __asm volatile
    (
        "MRS %[result],  CNTPCT_EL0": [result] "=r" (res)
    );
    return res;
}

int do_hello_cmd(void) 
{
    uart_puts("Hello World!\n");
    return 1;
}

int do_help_cmd() 
{
    uart_puts("help : print all available commands.#");
    uart_puts("hello : print Hello World!#"); 
    uart_puts("timestamp : print current timestamp.#");
    uart_puts("reboot : reboot.\n");
    return 1;
}

void reserve(char *str,int index)
{
    int i = 0, j = index - 1, temp;
    while (i < j) {
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp;
        i++;
        j--;
    }
}

void intToStr(int x, char *str) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
    reserve(str, i);
    str[i] = '\0';
    return ; 
} 

int do_time_cmd(void) 
{
    char inte_buf[50];
    char flot_buf[50];
    unsigned long long system_frq = get_system_frequency() ;
    unsigned long long system_count = get_system_count() ;
    double result =  (double)((double)system_count / (double)system_frq);
    int inte = (int)result;
    int flot = (int)((result - inte) * 100000);
    intToStr(inte, inte_buf);
    intToStr(flot, flot_buf);

    uart_puts(inte_buf);
    uart_send('.');
    uart_puts(flot_buf);
    uart_send('\n');
    return 1;
}

int do_rest_cmd() 
{
    reset();
    while(1);
    return 1;
}

int sstrcmp(char *array_1, char *array_2) {
    int i = 0;
    while(array_1[i] != '\0') {
        if (array_1[i] != array_2[i]) {
            return -1;
        }
        i++;
    }
    if(array_2[i] != '\0') {
        return -1;
    }
    return 0;
}

int process_cmd(char *command)
{
    if(!(sstrcmp(command, "hello"))) {
        do_hello_cmd();
        return 1;
    }
    if(!(sstrcmp(command, "help"))) {
        do_help_cmd();
        return 1;    
    }
    if(!(sstrcmp(command, "timestamp"))) {
        do_time_cmd();
        return 1;
    }
    if(!(sstrcmp(command, "reboot"))) {
        do_rest_cmd();
        return 1;
    }
    return 0;
}


void main()
{
    // set up serial console
    uart_init();
    
    // say hello
    uart_puts("Hello World!\n");
    uart_getc();
    char command_buffer[51];
    char *ptr_cmd = command_buffer;
    // echo everything back
    while(1) {
	    char g = uart_getc();
        if (g != '\n') {
            *ptr_cmd++ = g;
        }
        else {
            *ptr_cmd = '\0';
            if (!process_cmd(command_buffer)) {
                uart_puts("process command error\n");
            }
            ptr_cmd = command_buffer;
        }
    }
}
