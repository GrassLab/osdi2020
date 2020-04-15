#include "uart.h"
#include "mailbox.h"
#include "exception.h"

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
	if(frac_part<10)uart_send('0');
	if(frac_part<100)uart_send('0');
	if(frac_part<1000)uart_send('0');
	if(frac_part<10000)uart_send('0');
	uart_send_int(frac_part);
	uart_send(']');
	uart_send('\r');
	uart_send('\n');
	unsigned int a=~0;
	unsigned long b=~0;
	uart_send_int(a);
	uart_send('\r');
	uart_send('\n');
	
	uart_send_int(b);
	uart_send('\r');
	uart_send('\n');
//	uart_send_int(~0);
//	uart_send('\n');
}


void shell(){
	char r;
	char buffer[255];
	int ptr=0,current_el=0;
	buffer_clean(buffer);
	uart_send('#');
	uart_send(32);
	
	while(1){
		r=uart_getc();
		buffer[ptr++]=r;
		uart_send(r);
		if(r=='\n'){
			uart_send('\r');
			if(is_command(buffer, "hello\n")){
				uart_puts("Hello World!\n",0);
			}
			else if(is_command(buffer, "help\n")){
				uart_puts("exc : synchronous exception\n",0);
				uart_puts("irq : interrupt (enable timer)\n",0);
				uart_puts("help : help\n",0);
				uart_puts("hello : print Hello World!\n",0);
				uart_puts("mailbox : get board revision and vc memory\n",0);
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
			else if(is_command(buffer, "mailbox\n")){
				get_board_revision();	
				get_vc_memory();
			}
			else if(is_command(buffer, "exc\n")){
				int a=1,b=2,c=3,d=4,e=5,f=6;
				uart_hex(a);uart_hex(b);uart_hex(c);
				//asm volatile("bl save_all\n":::);	
				asm volatile("svc 1\n":::);
				uart_puts("End exc\n",0);
				uart_hex(a);uart_hex(b);uart_hex(c);			
			}
			else if(is_command(buffer, "brk\n")){
				asm volatile("brk 1\n":::);
				uart_puts("End brk\n",0);			
			}
			else if(is_command(buffer, "irq\n")){
				asm volatile("svc 5\n":::);
				interrupt_enable();
			}
			else if(is_command(buffer,"ex_level\n")){
				current_el=get_current_exception_level(1);			
			}
			else if(is_command(buffer,"switch1\n")){
				asm volatile("bl from_el2_to_el1\n":::);
				current_el=get_current_exception_level(1);
			}
			else if(is_command(buffer,"switch0\n")){
				asm volatile("bl from_el1_to_el0\n":::);
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
