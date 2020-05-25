#include "system.h"
#include "util.h"
#include "uart.h"
#include "mbox.h"
#include "loadimg.h"
#include "timer.h"
#include "irq.h"
#include "asm.h"
#include "shed.h"
#include "sys.h"
#include "unistd.h"

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

// void foo(){
//   int tmp = 5;
//   uart_puts("Task ");
//   uart_print_int(get_taskid());
//   uart_puts(" after exec, tmp address 0x");
//   uart_print_int(&tmp);
//   uart_puts(", tmp value ");
//   uart_print_int(tmp);
//   uart_puts("\r\n");
// //   printf("Task %d after exec, tmp address 0x%x, tmp value %d\n", get_taskid(), &tmp, tmp);
//   exit(0);
// }

void test() {
	
  int cnt = 1;
  int f;
  f = fork();
  if (f == 0) {
    fork();
    delay(100);
    fork();
    uart_puts("CNT value: ");
    uart_print_int(cnt);
    uart_puts("\r\n");
    while(cnt < 10) {
		uart_puts("Task id: ");
		uart_print_int(get_taskid());
		uart_puts(", cnt: ");
		uart_print_int(cnt);
		uart_puts("\r\n");
		// printf("Task id: %d, cnt: %d\n", get_taskid(), cnt);
		delay(100000);
		++cnt;
    }
    exit(0);
	uart_puts("Should not be printed\r\n");
    // printf("Should not be printed\n");
  } else {
	  	uart_puts("Task ");
		uart_print_int(get_taskid());
		uart_puts(" before exec, cnt address 0x");
		uart_print_int(&cnt);
		uart_puts(", cnt value ");
		uart_print_int(cnt);
		uart_puts("\r\n");
		// printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", get_taskid(), &cnt, cnt);
		// exec(foo);
  }
}

// -----------above is user code-------------
// -----------below is kernel code-------------

void user_test(){
  uart_puts("@ EL ");
  uart_print_int(get_el());
  uart_puts("\r\n");
  exec(test);
}

void foo(){
  while(1) {
    uart_puts("Task id: ");
    uart_print_int(current -> task_id);
    uart_puts("\n");
    delay(1000000);
    schedule();
  }
}

void idle(){
  while(1){
    // Schedule();
    delay(1000000);
  }
}

void kernel_main() {
  // ...
  // boot setup
  // ...
  uart_init();
  uart_getc();
  uart_puts("MACHINE IS OPEN!!\n");

  unsigned long current_el;
  current_el = get_el();
  uart_puts("Current EL: ");
  uart_hex(current_el);

  int N=5;
  for(int i = 0; i < N; ++i) { // N should > 2
    privilege_task_create(foo );
  }
  schedule();

  idle();
}

void kernel_main_() {
  // ...
  // boot setup
  // ...
  uart_init();
  uart_puts("Kernel process started. EL ");
  uart_print_int(get_el());
  uart_puts("\r\n");
  
  asm volatile ("mov x0, #0\n" "svc #0\n");

  init_idle_task(task[0]);

  privilege_task_create(user_test);
  schedule();
  // do_exec(user_test);
//   privilege_task_create(user_test);

  idle();
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
