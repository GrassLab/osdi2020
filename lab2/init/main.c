#include "buildin.h"
#include "console.h"
#include "kernel.h"
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "io.h"
#include "stdint.h"
#include "framebuffer.h"

void welcome_msg()
{
	printk("\r  ___  ____  ____ ___ ____   ___ ____   ___               \n"
	       " / _ \\/ ___||  _ \\_ _|___ \\ / _ \\___ \\ / _ \\        \n"
	       "| | | \\___ \\| | | | |  __) | | | |__) | | | |           \n"
	       "| |_| |___) | |_| | | / __/| |_| / __/| |_| |             \n"
	       " \\___/|____/|____/___|_____|\\___/_____|\\___/           \n"
	       "   Simple Raspberry Pi3 B+ bare metal shell v0.1          \n");
}

void init()
{
	timmer_init(); // initial timer
	uart_init(); // initial mini uart
	char test = "a";
test:
	printk("Kernel address: %p\n", &&test);
	// /* check if there is any pending loader */
	// uart_send(0x06)
	// uart_
	// fb_init();
	// fb_show();
}
int main()
{
	init();
	welcome_msg();

	char buf[1024];
	while (1) {
		printk("njt@osdi2020# ");
		readline(buf);
		if (*buf) {
			printk("\n");
			cmd(buf);
		} else {
			printk("\n");
		}
	}
}
