#include "uart.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024
#define CMD_NUM 4

static void foo(void) { uart_puts("foo()\n"); }
static void foo2(void) { uart_puts("foo22()\n"); }
static void exec_hello();
static void exec_help();
static void exec_boot();
static void exec_timestamp();
static void *_memset(void *dst, int val, int count);



struct cmd
{
    char name[10];
    char description[25];
    void(*callback)(void);
};

struct cmd cmd_list[] = { 
    { .name = "hello", .description = " Show commands available.", .callback = exec_hello}, 
    { .name = "help", .description = " Show \"Hello World!\"", .callback = exec_help}, 
    { .name = "boot", .description = " Get current timestamp.", .callback = exec_boot}, 
    { .name = "timestamp", .description = " Reboot device.", .callback = exec_timestamp}, 
};

void exec_boot(){
    uart_puts("rebooting ...");
	_reboot(1);
	while(1);
}

void _unsign_arr_to_digit(unsigned num, char* buf, unsigned len){
	for(int i=len-1; i>=0; i--){
		buf[i] = (char)(num%10 + '0');
		num /= 10;
	}
}

void exec_timestamp(){
	unsigned int time, timer_counter, timer_freq;
	char buf[10];
    _memset(buf,'\0',10);
	asm volatile("mrs %0, cntpct_el0": "=r"(timer_counter)::); 
	asm volatile("mrs %0, cntfrq_el0": "=r"(timer_freq)::);
	time = timer_counter / (timer_freq / 100000U);
	
	_unsign_arr_to_digit((time/100000U), buf, 3);
	uart_puts(buf); // natural part
	uart_send('.');
	_unsign_arr_to_digit(time%100000U, buf, 5);
	uart_puts(buf); // decimal part
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
        uart_send('\n');
    }
}

void reset(int tick){ // reboot after watchdog timer expire
  set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() {
  set(PM_RSTC, PM_PASSWORD | 0); // full reset
  set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}

int _strlen(char * str){
    char *s;
    for (s = str; *s; ++s)
        ;
    return (s - str);
}

int _compare_input(char * input){
    for(int i = 0; i < CMD_NUM; i++) {
        int s;
        for (s = 0; cmd_list[i].name[s] == input[s] && cmd_list[i].name[s] && input[s]; s++)
            ;
        if(s == _strlen(cmd_list[i].name) && input[s] == '\0'){
            cmd_list[i].callback();
            return 1;
        }
    }
    return 0;
}

void *_memset(void *dst, int val, int count)
{
	char *ptr = dst;

	while (count--)
		*ptr++ = val;

	return dst;
}

void _print(char * c){
    char *s;
    for (s = c; *s; ++s)
        uart_send(*s);
    uart_send("\n");
}

void main()
{
    // set up serial console
    uart_init();
    char buff[50];
    _memset(buff, 0, 50);    
    char *buff_ptr = buff;

    uart_puts("Hello World!\n");
    uart_puts("12345678910\n");  
    uart_send('#');
    while(1) {
        char c;
        if(c = uart_getc()){
            uart_send(c);
            
            if(c=='\n'){
                if(!_compare_input(buff))
                    uart_puts("Err: \n");
                uart_send('#');
                _memset(buff, '\0', 50);    
                buff_ptr = buff;
            }
            else
                *buff_ptr++ = c;
        }    
    }
}
