#include "str.h"
#include "uart.h"




void cal_time(unsigned long long int time_FRQ, unsigned long long int time_CT, char *char_time)
{
	int tmp,count=0;
	my_itoa((int)(time_CT / time_FRQ), char_time, 10);
	my_strcat(char_time, ".");

	tmp = (time_CT % time_FRQ)*10;
	char *p = char_time + my_strlen(char_time);
	while(tmp>0 && count < 10){
		*p++ = (char)((tmp / time_FRQ)+48);
		tmp = (tmp % time_FRQ)*10;
		count++;
	}
	*p = '\0';
}

void timestamp()
{
	//note : I didn't save register x0~x2, I think it is fine.
	char timer[100] = {0};
	unsigned long long int time_FRQ, time_CT;
	asm volatile("mov x1, %0" ::"r"(&time_FRQ):);
	asm volatile("mov x2, %0" ::"r"(&time_CT):);
	asm volatile("mov x0, %0" ::"r"(1):);
	asm volatile("svc #0");

	cal_time(time_FRQ, time_CT, timer);
	uart_puts("timestamp: ");
	uart_puts(timer);
	uart_puts("\n");
}

void core_time_enable()
{
	asm volatile("mov x0, 0\r\n" "svc #0");
}

void reboot()
{
	asm volatile("mov x0, %0\r\n" "svc #0" ::"r"(2):);
}

void show_svc_info()
{
	unsigned long long par[5]; // Exc Class, ISS, return address, current EL, current SP
	asm volatile("mov x0, %0" ::"r"(1):);
	asm volatile("mov x1, %0" ::"r"(par):);
	asm volatile("svc #1");
	uart_puts("Exc Class: ");
    uart_hex(par[0]);
    uart_puts("\r\nISS: ");
    uart_hex(par[1]);
    uart_puts("\r\nret addr: ");
    uart_hex(par[2]);
    uart_puts("\r\ncurrentEL: ");
    uart_hex(par[3]);
    uart_puts("\r\ncurrent SP: ");
    uart_hex(par[4]);
    uart_puts("\r\n");
}

