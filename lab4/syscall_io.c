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

/*void sys_num(unsigned int num, int base) 
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
}*/

void my_printf(const char *fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    syscall(12, fmt, args);
    __builtin_va_end(args);
}