#include "uart.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int*)(0x3F100024))

int strcmp(char *s1, char *s2){
    while(*s1 || *s2){
        if(*s1 != *s2)
            return 0;
        ++s1; ++s2;
    }
    return 1;
}

double get_time(){
    register double frequency, counter;
    double time;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(frequency));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(counter));
    time = counter / frequency;
    return time;
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset(){
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

int atoi(char *str){
    int res = 0; 
    for (int i = 2; str[i] != '\0'; ++i) 
        res = res * 16 + str[i] - '0';  
    return res; 
}

void print_next_instr_addr(){
    unsigned long lr;
    asm volatile ("mov %0, lr" :"=r"(lr));
    uart_hex(lr); 
}

void show_boot_msg(){
    uart_puts("  _   _  U _____ u  _       _       U  ___ u       ____      ____             _____    _   \n"    
              "|'| |'| \\| ___\"|/ |\"|     |\"|       \\/\"_ \\/    U |  _\"\\ u U|  _\"\\ u  ___    |___\"/u U|\"|u  \n"
              "/| |_| |\\ |  _|\" U | | u U | | u     | | | |     \\| |_) |/ \\| |_) |/ |_\"_|   U_|_ \\/ \\| |/ \n" 
              "U|  _  |u | |___  \\| |/__ \\| |/__.-,_| |_| |      |  _ <    |  __/    | |     ___) |  |_|  \n" 
              "|_| |_|  |_____|  |_____| |_____|\\_)-\\___/       |_| \\_\\   |_|     U/| |\\u  |____/   (_)   \n"
              "//   \\  <<   >>  //  \\  //  \\      \\         //   \\_  ||>>_.-,_|___|_,-._// \\  |||_  \n"
              "(_\") (\"_)(__) (__)(_\")(\"_)(_\")(\"_)    (__)       (__)  (__)(__)__)\\_)-' '-(_/(__)(__)(__)_)\n\n");
}
