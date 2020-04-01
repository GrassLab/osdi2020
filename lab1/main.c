#include "uart.h"
#define PM_PASSWORD 	((volatile unsigned int)(0x5a000000))

#define PM_RSTC		((volatile unsigned int*)(0x3F10001c))

#define PM_WDOG		((volatile unsigned int*)(0x3F100024))

void buffer_clean(char *buffer){
	int i=255;
	while(i--)buffer[i]=0;
}
int is_command(char *s, const char *cmd){
	int i=0;
	while(1){
		if(s[i]!=cmd[i])return 0;
		if(cmd[i]=='\n')return 1;
		i++;
	}
}
void reset(int tick){
	*PM_RSTC = PM_PASSWORD|0x20;
	*PM_WDOG = PM_PASSWORD|tick;
}
void cancel_reset(){
	*PM_RSTC = PM_PASSWORD|0;
	*PM_WDOG = PM_PASSWORD|0;
}
void print_timestamp(){
	unsigned long time_cnt=0,time_freq=1,int_part=0,frac_part=0;
	asm volatile("mrs %0, cntfrq_el0\n":"=r"(time_freq):"r"(time_freq):);
	asm volatile("mrs %0, cntpct_el0\n":"=r"(time_cnt):"r"(time_cnt):);
	int_part=time_cnt/time_freq;
	frac_part=(time_cnt%time_freq)*100000/time_freq;
//	uart_send_int(time_cnt);
//	uart_send(32);
//	uart_send_int(time_freq);
//	uart_send('\n');
	uart_send('[');
	uart_send_int(int_part);
	uart_send('.');
	if(frac_part<1)uart_send('0');
	else if(frac_part<10)uart_send('0');
	else if(frac_part<100)uart_send('0');
	else if(frac_part<1000)uart_send('0');
	else if(frac_part<10000)uart_send('0');
	uart_send_int(frac_part);
	uart_send(']');
	uart_send('\r');
	uart_send('\n');
//	uart_send_int(~0);
//	uart_send('\n');
}
void main(){
	char r;
	char buffer[255];
	int ptr=0;
	long int time_cnt=0,time_freq=1,int_part,frac_part;
	uart_init();
	//cancel_reset();
	buffer_clean(buffer);
	//uart_puts("HELLO WORLD!\n",0);
	print_figlet();
	uart_send('#');
	uart_send(32);
	while(1){
		r=uart_getc();
		buffer[ptr++]=r;
		uart_send(r);
		if(r=='\n'){
			uart_send('\r');
			if(is_command(buffer, "hello\n"))
				uart_puts("Hello World!\n",0);
			else if(is_command(buffer, "help\n")){
				uart_puts("hello : print Hello World!\n",0);
				uart_puts("help : help\n",0);
				uart_puts("reboot : reboot rpi3\n",0);
				uart_puts("timestamp : get current timestamp\n",0);
			}
			else if(is_command(buffer,"timestamp\n")){
				print_timestamp();
			}
			else if(is_command(buffer, "reboot\n")){
				cancel_reset();
				reset(10);
			}
			else{
				uart_puts("Err: command ",13);
				uart_puts(buffer,ptr-1);
				uart_puts(" not found, try <help>\n",0);
			}
			uart_send('#');
			uart_send(32);
			buffer_clean(buffer);
			ptr=0;
		}	
	}
}
