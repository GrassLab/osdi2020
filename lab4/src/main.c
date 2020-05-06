#include "system.h"
#include "util.h"
#include "uart.h"
#include "mbox.h"
#include "loadimg.h"
#include "timer.h"
#include "irq.h"
#include "asm.h"
#include "shed.h"

#define	CMD_HELP	"help"
#define	CMD_HELLO	"hello"
#define	CMD_TIME	"timestamp"
#define	CMD_REBOOT	"reboot"
#define CMD_LOADIMG	"loadimg"
#define CMD_PICTURE "picture"
#define CMD_EXC		"exc"
#define CMD_IRQ		"irq"
#define CMD_BRK		"brk"

void get_timestamp();

void _foo() {
	while(1) {
		struct task_struct *current = get_current_task();
		uart_puts("Task id: ");
		uart_print_int(current -> task_id);
		uart_puts("\r\n");
		delay(1000000000);
		schedule();
	}
}

void foo(){
	do_exec(_foo);
	// while(1) {
	// 	struct task_struct *current = get_current_task();
	// 	uart_puts("Task id: ");
	// 	uart_print_int(current -> task_id);
	// 	uart_puts("\r\n");
	// 	delay(1000000000);
	// 	schedule();
	// }
}

void idle(){
	while(1){
		schedule();
		delay(1000000);
	}
}

void main() {
	uart_init();
	uart_puts("hihi\r\n");
	asm volatile ("mov x0, #0\n" "svc #0\n");
	

	for(int i = 0; i < 10; ++i) { // N should > 2
		// uart_puts("hihi\r\n");
		privilege_task_create(foo);
	}

	// privilege_task_create(foo);

	schedule();
	// while (1){
	// 	schedule();
	// }
}

// void main()
// {
// 	uart_init();
// 	// char *welcome = " _ _ _   _   __  ___   __  _ _  _  _ _ \n| | | | / \\ |  \\| __| / _|| U || || U |\n| V V || o || o ) _|  \\_ \\|   || ||   |\n \\_n_/ |_n_||__/|___| |__/|_n_||_||_n_|\n\n";
// 	char *welcome = "\n                        ___                  _ _ _        \n                       |__ \\                (_|_) |       \n   _ __ _   _ _ __ ___    ) |_ __ ___   ___  _ _| |_ ___  \n  | '__| | | | '_ ` _ \\  / /| '_ ` _ \\ / _ \\| | | __/ _ \\ \n  | |  | |_| | | | | | |/ /_| | | | | | (_) | | | || (_) |\n  |_|   \\__,_|_| |_| |_|____|_| |_| |_|\\___/| |_|\\__\\___/ \n                                           _/ |           \n                                          |__/            \n";
// 	uart_puts(welcome);

// 	get_board_revision();
// 	get_vc_core_base_addr();
// 	//local_timer_init();

// 	while(1) {
// 		uart_puts("#");
// 		char command[20];
// 		uart_read_line(command);
// 		uart_puts("\r");
// 		if(strcmp(command, CMD_HELP)) {
// 			char *help = "help:\t\t help";
// 			char *hello = "hello:\t\t print Hello World!";
// 			char *timestamp = "timestamp:\t get current timestamp";
// 			char *reboot = "reboot:\t\t reboot rpi3";
// 			char *loadimg = "loadimg:\t load image to rpi3";
// 			char *exc = "exc:\t\t svc #1";
// 			uart_puts(help);
// 			uart_puts("\r\n");
// 			uart_puts(hello);
// 			uart_puts("\r\n");
// 			uart_puts(timestamp);
// 			uart_puts("\r\n");
// 			uart_puts(reboot);
// 			uart_puts("\r\n");
// 			uart_puts(loadimg);
// 			uart_puts("\r\n");
// 			uart_puts(exc);
// 		} else if(strcmp(command, CMD_HELLO)) {
// 			uart_puts("Hello World!");
// 		} else if(strcmp(command, CMD_LOADIMG)) {
// 			uart_puts("[rpi3]\tStart Loading kernel image...\r\n");
// 			uart_puts("[rpi3]\tPlease input kernel load address (press <Enter> to use default addr: 0x80000):\r\n");
// 			char input_addr[20];
// 			uart_read_line(input_addr);
// 			if(strcmp(input_addr, "")) {
// 				input_addr[20] = "80000";
// 			}
// 			char res_str[100];
			
// 			load_image(input_addr);
// 		}  else if(strcmp(command, CMD_TIME)) {
// 			get_timestamp();
// 		}  else if(strcmp(command, CMD_REBOOT)) {
// 			reset();
// 		}  else if(strcmp(command, CMD_PICTURE)) {
// 			lfb_init();
// 			lfb_showpicture();
// 		} else if(strcmp(command, CMD_EXC)) {
// 			//int i = get_el();
// 			//uart_print_int(i);
// 			asm volatile ("svc	#1");
// 			//i = get_el();
// 			//uart_print_int(i);
// 		} else if(strcmp(command, CMD_BRK)) {
// 			//int i = get_el();
// 			//uart_print_int(i);
// 			asm volatile ("brk	#1");
// 			//i = get_el();
// 			//uart_print_int(i);
// 		} else if(strcmp(command, CMD_IRQ)) {
// 			// asm volatile(" mov     x0, #0");
// 			// asm volatile("msr     DAIF, x0");
// 			// local_timer_init();
// 			// core_timer_init();
// 			asm volatile ("mov x0, #0\n" "svc #0\n");
// 		} else {
// 			uart_puts("ERROR: ");
// 			uart_puts(command);
// 			uart_puts(" command not found! try <help>");
// 		}

// 		uart_puts("\r\n");
// 	}
// }

void get_timestamp()
{
	register unsigned long f, c;
	asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
	asm volatile ("mrs %0, cntpct_el0" : "=r"(c));
	char res[30];
	ftoa(((float)c/(float)f), res, 10);
	uart_puts(res);
}
