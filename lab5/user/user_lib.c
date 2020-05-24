#include "user_lib.h"
#include "str.h"
#define MMIO_BASE       0x3F000000
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))

void core_time_enable()
{
	asm volatile("mov x0, 0\r\n" "svc #0");
}
void reboot()
{
	asm volatile("mov x0, %0\r\n" "svc #0" ::"r"(2):);
}

void show_hex(unsigned long long a)
{
	syscall(SYSN_UART_HEX, a);
}

void puts(char *c) //for user
{
	syscall(SYSN_UART_WRITE, c);
}

void send(char c)
{
    syscall(SYSN_UART_SEND, c);
}

void exec(void(*func)())
{
	syscall(SYSN_EXEC, func);
}

int fork()
{
	return syscall(SYSN_FORK);
}

void exit(int x)
{
    syscall(SYSN_EXIT, x);
}

char getc() //for user
{
	char c = (char)syscall(SYSN_UART_READ);
	return c;
}

void schedule()
{
	syscall(SYSN_SCHEDULE);
}

int get_task_id()
{
    return (int)syscall(SYSN_GET_TASK_ID);
}

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
	puts("timestamp: ");
	puts(timer);
	puts("\n");
}

void sys_num(unsigned int num, int base) 
{ 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0); 

    puts(ptr);
}

void printf(const char *fmt, ...)
{
	static char mutex = 0;
	while(mutex){
		asm volatile("nop");
	}
	mutex = 1;
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    const char *traverse; 
	//unsigned int i; 
	//char *s; 
	for(traverse = fmt; *traverse != '\0'; traverse++) 
	{ 
        if(*traverse == '%')
        {
            traverse++;
            switch(*traverse)
            {
                case '%':
                    send('%');
                    break;
                case 's':
                    //char *p = va_arg(args, char *);
                    puts(__builtin_va_arg(args, char *));
                    break;
                case 'd':
                    //int arg = va_arg(args, int);
                    sys_num(__builtin_va_arg(args, int), 10);
                    break;
                case 'x':
                    //int arg = va_arg(args, int);
                    sys_num(__builtin_va_arg(args, int), 16);
                    break;
            }
        }
        else
        {
            send(*traverse);
        }
    }
    __builtin_va_end(args);
	mutex = 0;
}

void delay()
{
	unsigned long long size = Idle_size;
	while(size--){asm volatile("nop");}
}

void uart_send(unsigned int c) {
    
    /*if(*UART0_FR&0x20)
    {
        //tran_buf.tail++;
        tran_buf.buf[tran_buf.tail++] = c;
    }
    else
    {
        *UART0_DR=c;
    }*/

    /*-----------------old version for test-----------------*/
    do{asm volatile("nop");}while(*UART0_FR&0x20);
    *UART0_DR=c;

}

void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}
