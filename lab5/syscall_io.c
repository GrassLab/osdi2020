#include "uart.h"

int syscall();

char sys_read() //for user
{
	char c = (char)syscall(5);
	return c;
}

void sys_puts(char *c) //for user
{
	syscall(6, c);
}

void sys_send(char c)
{
    syscall(11, c);
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

    sys_puts(ptr);
}

void my_printf(const char *fmt, ...)
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
                    uart_send('%');
                    break;
                case 's':
                    //char *p = va_arg(args, char *);
                    uart_puts(__builtin_va_arg(args, char *));
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
            uart_send(*traverse);
        }
    }
    __builtin_va_end(args);
	mutex = 0;
}