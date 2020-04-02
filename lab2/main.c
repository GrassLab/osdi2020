#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
	*PM_RSTC = (PM_PASSWORD | 0x20);
	*PM_WDOG = (PM_PASSWORD | tick);
	/*set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick*/
}

void cancel_reset() {
	*PM_RSTC = (PM_PASSWORD | 0);
	*PM_WDOG = (PM_PASSWORD | 0);
	/*set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick*/
}


void myreadline(char *str, int max_size)
{
	int count=0;
	char input_c;
	while(1){
		if(count > max_size)
			break;
		input_c = uart_getc();
		if(input_c == '\n' || input_c=='\0')
		{
			uart_puts("\r\n");
			break;
		}
		uart_send(input_c);
		str[count] = input_c;
		count++;
	}
}


void cal_time(volatile long long int time_FRQ, volatile long long int time_CT, char *char_time)
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
	char timer[100] = {0};
	volatile unsigned long long int time_FRQ, time_CT;
	asm volatile("mrs %0, CNTFRQ_EL0" : "=r"(time_FRQ) ::);
	asm volatile("mrs %0, cntpct_el0" : "=r"(time_CT)::);
	cal_time(time_FRQ, time_CT, timer);
	uart_puts(timer);
	uart_puts("\n");
}

void reboot()
{
	reset(1000);
}

void get_VC_base()
{
	volatile unsigned int  __attribute__((aligned(16))) mailbox[8];
	mailbox[0] = 7 * 4; // buffer size in bytes
	mailbox[1] = REQUEST_CODE;
	// tags begin
	mailbox[2] = 0x00010006; // tag identifier
	mailbox[3] = 8; // maximum of request and response value buffer's length.
	mailbox[4] = TAG_REQUEST_CODE;
	mailbox[5] = 0; // clear buffer
	mailbox[6] = 0;
	// tags end
	mailbox[7] = END_TAG;
	mbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	uart_puts("VC memory base address in bytes: ");
	uart_hex(mailbox[5]);
	uart_puts("\nVC memory size in bytes: ");
	uart_hex(mailbox[6]);
	uart_puts("\n");
}

void get_board_revision()
{
	volatile unsigned int  __attribute__((aligned(16))) mailbox[8];
	mailbox[0] = 7 * 4; // buffer size in bytes
	mailbox[1] = REQUEST_CODE;
	// tags begin
	mailbox[2] = GET_BOARD_REVISION; // tag identifier
	mailbox[3] = 4; // maximum of request and response value buffer's length.
	mailbox[4] = TAG_REQUEST_CODE;
	mailbox[5] = 0; // value buffer
	// tags end
	mailbox[6] = END_TAG;
	mbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	uart_puts("board revision: ");
	uart_hex(mailbox[5]);
	uart_puts("\n");
}

void lfbuf()
{
	lfb_init();
	lfb_showpicture();
}

void help()
{
	uart_puts("help : help\n");
	uart_puts("hello : print Hello World!\n");
	uart_puts("timestamp : get current timestamp\n");
	uart_puts("reboot : reboot system\n");
	uart_puts("vcb : VC Core base address.\n");
	uart_puts("brv : board revision.\n");
	uart_puts("lfb : frame buffer.\n");
}

void main()
{
	
    // set up serial console
    uart_init();
	
	uart_puts("Hello~~ try 'help' \n");
	char *command[9] = {"help", "hello", "timestamp", "reboot", "vcb", "brv", "lfb", "address", 0};
	//uart_hex((unsigned int)command);
    char input[100]={0};
	
    while(1){
		my_strset(input, 0, 100);
		uart_puts(">> ");
		myreadline(input, 100);
		if(my_strcmp(input, command[0], my_strlen(input)) == 0) //b *0x20af8  (0x20aec:     add     x1, x20, #0xde8)
        {	
			help();
        }
        else if(my_strcmp(input, command[1], my_strlen(input)) == 0)
        {
            uart_puts("Hello World!\n");
        }
		else if(my_strcmp(input, command[2], my_strlen(input)) == 0)
		{
			timestamp();
		}
		else if(my_strcmp(input, command[3], my_strlen(input)) == 0)
		{
			reboot();
		}
		else if(my_strcmp(input, command[4], my_strlen(input)) == 0)
		{
			get_VC_base();
		}
		else if(my_strcmp(input, command[5], my_strlen(input)) == 0)
		{
			get_board_revision();
		}
		else if(my_strcmp(input, command[6], my_strlen(input)) == 0)
		{
			lfbuf();
		}
		else if(my_strcmp(input, command[7], my_strlen(input)) == 0)
		{
sunnn:
			uart_hex((unsigned long)&&sunnn);
		}
		else
		{
			uart_puts("Err command ");
			uart_puts(input);
			uart_puts(" not found, try <help>");
			uart_puts("\n");
		}
	}
	
}