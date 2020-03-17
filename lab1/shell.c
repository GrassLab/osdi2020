#include "shell.h"

struct cmd
{
    char name[10];
    char description[30];
    void(*callback)(void);
};

struct cmd cmd_list[] = { 
    { .name = "hello", .description = " Show commands available.", .callback = exec_hello}, 
    { .name = "help", .description = " Show \"Hello World!\"", .callback = exec_help}, 
    { .name = "boot", .description = " Get current timestamp.", .callback = exec_boot}, 
    { .name = "timestamp", .description = " Reboot device.", .callback = exec_timestamp}, 
};


void exec_boot(){
    uart_puts("rebooting\n");
	reset(100);
	while(1);
}

void exec_timestamp(){
	unsigned int time, timer_counter, timer_freq;
	char buf[10];
    _memset(buf,'\0',10);
	asm volatile("mrs %0, cntpct_el0": "=r"(timer_counter)::); 
	asm volatile("mrs %0, cntfrq_el0": "=r"(timer_freq)::);
	time = timer_counter / (timer_freq / 100000U);
	
	_unsign_arr_to_digit((time/100000U), buf, 3);
	uart_send('[');
	uart_puts(buf); 
	uart_send('.');
	_unsign_arr_to_digit(time%100000U, buf, 5);
	uart_puts(buf); 
	uart_send(']');
	uart_puts("\n");
}

void exec_hello(){
    uart_puts("Hello World!\n");
}

void exec_help(){
    for(int i = 0; i < CMD_NUM; i++) {
        uart_puts(cmd_list[i].name);
        uart_send(':');
        uart_puts(cmd_list[i].description);
        uart_puts("\n");
    }
}

void _unsign_arr_to_digit(unsigned num, char* buf, unsigned len){
	for(int i=len-1; i>=0; i--){
		buf[i] = (char)(num%10 + '0');
		num /= 10;
	}
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC =  (PM_PASSWORD | 0x20); // full reset
  *PM_WDOG =  (PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset(){
  *PM_RSTC = (PM_PASSWORD | 0); // full reset
  *PM_WDOG = (PM_PASSWORD | 0); // number of watchdog tick
}

int _compare_input(char * input){
    for(int i = 0; i < CMD_NUM; i++) {
        int s;
        for (s = 0; cmd_list[i].name[s] == input[s] && input[s] && cmd_list[i].name[s]; s++)
            ;
        if(s == _strlen(cmd_list[i].name) && (!input[s]) ){
            cmd_list[i].callback();
            return 1;
        }
    }
    return 0;
}
