#include "exec.h"

extern struct cmd
{
    char name[10];
    char description[30];
    void(*callback)(void);
};

extern struct cmd cmd_list[5] = { 
    { .name = "hello", .description = " Show commands available.", .callback = exec_hello}, 
    { .name = "help", .description = " Show \"Hello World!\"", .callback = exec_help}, 
    { .name = "boot", .description = " Get current timestamp.", .callback = exec_boot}, 
    { .name = "timestamp", .description = " Reboot device.", .callback = exec_timestamp}, 
    { .name = "send", .description = " send kernel img to raspi", .callback = copy_self_kernel}, 
};

int _compare_input(char * input){
    for(int i = 0; i < CMD_NUM; i++) {
        if(!strcmp(input,cmd_list[i].name)){
            cmd_list[i].callback();
            return 1;
        }
    }
    if(!strcmp(input,"haha")){
        exec_hello();
        exec_timestamp();
        return 1;
    }
    return 0;
}

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
	
	_unsign_arr_to_digit((time/100000U), buf, 5);
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

void exec_send(){
    int size = recv_img_size();
    uint8_t *kernel = (uint8_t*)0x10000;
    
    int check = 0;
    for (int i = 0; i < size-1; i++) {
        if (i % 10000 == 0){
            uart_puts("have sent 10000 bytes\n");
        }
        uint8_t c = uart_recv();
        *kernel++ = c;
        check += c;
    }

    uart_hex(kernel);
    uart_puts("\n");

    uart_puts("send finish\n");
    uart_send_int(check);
    // branch_to_address((unsigned long int *)0x10000);
    // void (*jump_new_kernel)(void) = new_address;
    // jump_new_kernel();
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC =  (PM_PASSWORD | 0x20); // full reset
  *PM_WDOG =  (PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset(){
  *PM_RSTC = (PM_PASSWORD | 0); // full reset
  *PM_WDOG = (PM_PASSWORD | 0); // number of watchdog tick
}
