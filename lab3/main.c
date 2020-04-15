#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#include "exc.h"
#include "syscall.h"


void myreadline(char *str, int max_size)
{
	int count=0;
	char input_c;
	while(1){
		if(count >= max_size)
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

void get_VC_base()
{
	_mbox[0] = 7 * 4; // buffer size in bytes
	_mbox[1] = REQUEST_CODE;
	// tags begin
	_mbox[2] = 0x00010006; // tag identifier
	_mbox[3] = 8; // maximum of request and response value buffer's length.
	_mbox[4] = TAG_REQUEST_CODE;
	_mbox[5] = 0; // clear buffer
	_mbox[6] = 0;
	// tags end
	_mbox[7] = END_TAG;
	mbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	uart_puts("VC memory base address in bytes: ");
	uart_hex(_mbox[5]);
	uart_puts("\nVC memory size in bytes: ");
	uart_hex(_mbox[6]);
	uart_puts("\n");
}

void get_board_revision()
{
	_mbox[0] = 7 * 4; // buffer size in bytes
	_mbox[1] = REQUEST_CODE;
	// tags begin
	_mbox[2] = GET_BOARD_REVISION; // tag identifier
	_mbox[3] = 4; // maximum of request and response value buffer's length.
	_mbox[4] = TAG_REQUEST_CODE;
	_mbox[5] = 0; // value buffer
	// tags end
	_mbox[6] = END_TAG;
	mbox_call(MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
	uart_puts("board revision: ");
	uart_hex(_mbox[5]);
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
	uart_puts("address : show now address and SP.\n");
	uart_puts("exc_svc : demo exception svc handler.(call svc #9)\n");
	uart_puts("exc_brk : demo exception brk handler.(call brk #1)\n");
	uart_puts("irq : enable core timer, local timer\n");
}

void main()
{
	
    // set up serial console
    uart_init();
	uart_puts("Hello~~ try 'help' \n");
	
	char *command[12] = {"help", "hello", "timestamp", "reboot", "vcb", "brv", "lfb", "address", "exc_svc", "exc_brk","irq", 0};
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
        else if(my_strcmp(input, command[1], my_strlen(input)) == 0) //Hello
        {
            uart_puts("Hello World!\n");
        }
		else if(my_strcmp(input, command[2], my_strlen(input)) == 0) //timestamp
		{
			timestamp(); //syscall
		}
		else if(my_strcmp(input, command[3], my_strlen(input)) == 0) //reboot
		{
			reboot(); //syscall
		}
		else if(my_strcmp(input, command[4], my_strlen(input)) == 0) //VC base
		{
			get_VC_base();
		}
		else if(my_strcmp(input, command[5], my_strlen(input)) == 0) //board_revision
		{
			get_board_revision();
		}
		else if(my_strcmp(input, command[6], my_strlen(input)) == 0) //frame buffer
		{
			lfbuf();
		}
		else if(my_strcmp(input, command[7], my_strlen(input)) == 0) // show address and stack pointer info
		{
sunnn:
			uart_puts("now address : ");
			uart_hex((unsigned long)&&sunnn);
			unsigned long sp_value;
			asm volatile("mov %0,sp":"=r"(sp_value)::);
			uart_puts("\r\nnow stack : ");
			uart_hex((unsigned long)sp_value);
			uart_puts("\r\n");
		}
		else if(my_strcmp(input, command[8], my_strlen(input)) == 0) //svc exception demo 
		{
			show_svc_info();
		}
		else if(my_strcmp(input, command[9], my_strlen(input)) == 0) // brk exception demo
		{
			asm volatile("brk #1");
		}
		else if(my_strcmp(input, command[10], my_strlen(input)) == 0)  //irq core timer and local timer enable
		{
			local_timer_init();
			//asm volatile("svc #0");
			core_time_enable();
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