#include "uart.h"
#include "mailbox.h"

#define input_buffer_Max 64


extern int gtime_frq(void);
extern int gtime_ct(void);

int strcmp(char *a, char *b)
{
	while(*a != '\0' || *b != '\n'){
		if(*a != *b) return -1;
		a++;
		b++;
	}
	return 0;
}

void help_cmd(void)
{
	uart_puts(" help      - print all available commands\n");
	uart_puts(" hello     - print Hello World!\n");
	uart_puts(" timestamp - print current timestamp\n");
	uart_puts(" hwinfo    - print hardware infomation\n");
	uart_puts(" exc       - issue [svc #1] and print return address,EC and ISS\n");
}

void hardware_info(void)
{

	get_board_revision();
	uart_puts("Board revision : 0x");
	uart_hex( mailbox[5] );
	uart_puts("\n");
	
	get_vc_memory();
	uart_puts("VC core base address : 0x");
	uart_hex( mailbox[5] );
	uart_puts("   size : 0x");
	uart_hex( mailbox[6] );
	uart_puts("\n");
}

void timestamp(void)
{
	float time=0.0,fpart;
	int i=0,tmp,f_n=7,ipart;
	char c[32];

	time = (float)gtime_ct() / (float)gtime_frq();
	ipart = (int)time;
	fpart =  time - (float)ipart;


	//uart_puts("[");
	while(ipart){
		c[i++]=(ipart%10) + '0';
		//uart_puts(&c);
		ipart/=10;
	}
	c[i++]='.';
	while(f_n-- > 0){
		c[i++]=(tmp=fpart*10) + '0';
		//uart_puts(&c);
		fpart=(fpart*10-(float)tmp);
	}
	c[i]='\0';

	uart_send('[');
	uart_puts(c);
	uart_send(']');
}
void main()
{
	char s[input_buffer_Max];
	int i=0,j;

	uart_init();
	
	uart_puts("# ");
	while(1){
		if(i==input_buffer_Max || s[i-1]=='\n'){

			if(i==input_buffer_Max) uart_puts("\n Error command length! \n");
			else if(strcmp("help", s) == 0) 
				help_cmd();
			else if(strcmp("hello", s) == 0)
				uart_puts(" Hello World! \n");
			else if(strcmp("exc", s) == 0)
				asm volatile ("svc #1");
			else if(strcmp("timestamp", s) == 0)
			{
				timestamp();
				uart_send('\n');
			}
			else if(strcmp("hwinfo", s) == 0)
				hardware_info();
			//else if(strcmp("loadimg", s) == 0)
			//	loadimg();
			else{
				s[i-1]='\0';
				uart_puts(" Error:command '");
				uart_puts(s);
				uart_puts("' not found, try <help>\n");
			}
			for(j=0;j<i;j++){
			       	s[j]=0;
			}

			uart_puts("# ");
			i=0;	
		}
		uart_send( ( s[i++] = uart_getc() ) );
	}
}

