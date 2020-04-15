#include "buildin.h"
#include "console.h"
#include "kernel.h"
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "io.h"
#include "stdint.h"
#include "framebuffer.h"

// struct pipe_t {
// 	uint8_t len = 0;
// 	uint8_t _buf[16];
// 	uint8_t (*put)(uint8_t);
// 	uint8_t (*get)(void);
// 	uint8_t (*isEmpty)(void);
// };
// struct pipe_t pipe;
// uint8_t put(uint8_t data)
// {
// 	if (pipe.len >= 16)
// 		return;
// 	pipe._buf[pipe.len++] = data;
// 	return len;
// }
// uint8_t get()
// {

// }
void local_timer_handler();
void core_timer_handler();

void irq_handler()
{
#define CORE0_INTR_SOURCE (uint32_t *)0x40000060
#define IRQ_BASIC_PENDING (uint32_t *)(MMIO_BASE + 0xb200)
#define UART0_RIS ((uint32_t *)(MMIO_BASE + 0x0020103c))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
	uint32_t core0_intid = *CORE0_INTR_SOURCE;
	uint32_t base_intid = *IRQ_BASIC_PENDING;
	static uint32_t core_timer_counter = 0;
	static uint32_t local_timer_counter = 0;
	if (base_intid & 0x80000) {
		if (*UART0_RIS & 0x10) {
			char r;
			send('R');
			/* UARTRXINTR */
			while (!(*UART0_FR & 0x10)) {
				r = (char)(*UART0_DR);
				// send(r);
				ring_buf_put(recv_buf, r);
			}
			*UART0_ICR |= 1 << 4;
		}
		if (*UART0_RIS & 0x20) {
			/* UARTTXINTR */
			char r;
			while ((r = ring_buf_get(trans_buf)) > 0) {
				while (*UART0_FR & 0x20)
					;
				*UART0_DR = r;
			}
			while (*UART0_FR & 0x20)
				;
			// *UART0_DR = r;
			*UART0_ICR |= 1 << 5;
		}
		if (*UART0_RIS & 0x40) {
			char r;
			while (!(*UART0_FR & 0x10)) {
				r = (char)(*UART0_DR);
				send(r);
				ring_buf_put(recv_buf, r);
			}
			*UART0_ICR |= 1 << 6;
		}
	}
	if (core0_intid & 0x2) {
		printk("Core timer interrupt %d times\n", core_timer_counter++);
		core_timer_handler();
	}
	if (core0_intid & 0x800) {
		/* local timer interrupt */
		printk("Local timer interrupt %d times\n",
		       local_timer_counter++);
		local_timer_handler();
	}
}

void undefine()
{
	uart_puts("UNDEFINE\n");
}

void init()
{
	uart_init();
	timmer_init();
	// /* check if there is any pending loader */
	// uart_send(0x06)
	// uart_
	// fb_init();
	// fb_show();
}

int main()
{
	init();
	//local_timer_init();
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
