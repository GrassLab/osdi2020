#include "kernel.h"
#include "timer.h"
#include "stdint.h"
#include "io.h"
#include "framebuffer.h"
#include "uart.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (unsigned int *)0x3F10001c
#define PM_WDOG (unsigned int *)0x3F100024
#define set(a, b) (*a = b)

#define GET_BOARD_REVISION 0x00010002
#define REQUEST_CODE 0x00000000
#define REQUEST_SUCCEED 0x80000000
#define REQUEST_FAILED 0x80000001
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

void hello()
{
	printk("Hello World!!\n");
}

void reset()
{ // reboot after watchdog timer expire
	float second = 1.5;
	printk("reset in %.2f second.\n", second);
	int wait_time = second * getFrequency();
	set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | wait_time); // number of watchdog tick
}

void cancel_reset()
{
	set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}

int get_serial(uint64_t *serial_num)
{
	uint32_t mailbox[8] = {
		8 * 4, MBOX_REQUEST, MBOX_TAG_GETSERIAL, 8, 8, 0,
		0,     MBOX_TAG_LAST
	};
	mbox_send(MBOX_CH_PROP, mailbox);
	int res = mbox_read(MBOX_CH_PROP, mailbox);
	*serial_num = mailbox[5];
	*serial_num = (*serial_num << 32) | mailbox[6];
	return res;
}
int get_videocore_memory(uint32_t *base_addr, uint32_t *size)
{
	uint32_t mailbox[8] = { 8 * 4, REQUEST_CODE,	 0x00010006,
				8,     TAG_REQUEST_CODE, 0,
				0,     END_TAG };
	mbox_send(MBOX_CH_PROP, mailbox);
	int res = mbox_read(MBOX_CH_PROP, mailbox);
	*base_addr = mailbox[5];
	*size = mailbox[6];
	return res;
}
void get_board_revision()
{
	uint32_t mailbox[7] = { 7 * 4,	REQUEST_CODE,	  GET_BOARD_REVISION,
				4,	TAG_REQUEST_CODE, 0,
				END_TAG };
	mbox_send(MBOX_CH_PROP, mailbox);
	int res = mbox_read(MBOX_CH_PROP, mailbox);
	if (res) {
		printk("Board number is %x.\n", mailbox[5]);
	} else {
		printk("Can't get board version.\n");
	}
}
void print_info()
{
	uint64_t serial_num;
	uint32_t base_addr, size;
	int serial_res = get_serial(&serial_num);
	int vc_mem_res = get_videocore_memory(&base_addr, &size);
	printk("Board: Raspberry Pi 3 B+\n");
	printk("OS: Bare Metal\n");
	printk("Maintainer: njt@2020\n");
	if (serial_res)
		printk("Serial num: %x\n", serial_num);
	// vc_mem_res &&printk("VC memory base: %x size: %x\n", base_addr, size);
	if (vc_mem_res) {
		printk("VC memory base: 0x");
		uart_hex(base_addr);
		printk(" size: 0x");
		uart_hex(size);
		printk("\n");
	}
	printk("CNTFRQ: %d\n", getFrequency());
	printk("CNTPCT: %d\n", getCurrentCount());
}

void getTimestamp()
{
	printk("[%.8f]\n", getCurrentTime());
}

int flag = 0;
void show_splash()
{
	if (!flag) {
		fb_init();
		flag = 1;
	}
	fb_show();
}

void exc()
{
	asm volatile("mov x0, #8\n\t"
		     "svc #0");
}
void core_timer_init();
void irq()
{
	core_timer_init();
}
void local_timer()
{
	local_timer_init();
}
