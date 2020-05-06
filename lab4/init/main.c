#include "buildin.h"
#include "console.h"
#include "kernel.h"
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "io.h"
#include "stdint.h"
#include "framebuffer.h"
#include "kernel/task.h"
#include "kernel/sched.h"

void local_timer_handler();
void core_timer_handler();
void enable_irq();
void disable_irq();
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
			/* UARTRXINTR */
			while (!(*UART0_FR & 0x10)) {
				r = (char)(*UART0_DR);
				// send(r);
				ring_buf_put(&recv_buf, r);
			}
			*UART0_ICR |= 1 << 4;
		}
		if (*UART0_RIS & 0x20) {
			/* UARTTXINTR */
			char r;
			while (!ring_buf_empty(&trans_buf)) {
				r = ring_buf_get(&trans_buf);
				while (*UART0_FR & 0x20) {
					asm volatile("nop");
				}
				*UART0_DR = r;
			}
			*UART0_ICR |= 1 << 5;
		}
		if (*UART0_RIS & 0x40) {
			char r;
			while (!(*UART0_FR & 0x10)) {
				r = (char)(*UART0_DR);
				// send(r);
				ring_buf_put(&recv_buf, r);
			}
			*UART0_ICR |= 1 << 6;
		}
		return;
	}
	if (core0_intid & 0x2) {
		print("Core timer interrupt %d times\n", core_timer_counter++);
		core_timer_handler();
		// asm volatile("mrs")
		enable_irq();
		uint32_t count = 0;
		while (count++ < 1000000) {
			asm volatile("nop");
		}
		disable_irq();
	}
	if (core0_intid & 0x800) {
		/* local timer interrupt */
		print("Local timer interrupt %d times\n",
		      local_timer_counter++);
		local_timer_handler();
	}
}

void undefine()
{
	uart_puts("UNDEFINE\n");
}

uint32_t sys_get_current_el()
{
	uint32_t el;
	asm volatile("mrs %0, currentEL" : "=r"(el)::);
	return el >> 2;
}
void delay(uint32_t tick)
{
	while (tick--) {
		asm volatile("nop");
	}
}
void foo()
{
	int tmp = 5;
	print("Task %d after exec, tmp address 0x%x, tmp value %d\n",
	      current_task, &tmp, tmp);
	while (1) {
		// uint64_t sp;
		// asm volatile("mov %0, sp" : "=r"(sp)::);
		// print("Task id: %d with sp %x ",
		//       task_pool[current_task].task_id, sp);
		// print("in EL%d ", sys_get_current_el());
		// print("with level %d\n", task_pool[current_task].privilege);
		print("haha\n");
		for (int i = 10; i < 100; i++)
			delay(1000000);
		// sched_next();
	}
}
int do_sys(int num);
void schedule()
{
	// do_sys(9);
	asm volatile("mov x0, #9\n\t"
		     "svc #0");
}
void idle()
{
	while (1) {
		schedule();
		uart_puts("Idle\n");
		delay(1000000);
	}
}
void init()
{
	uart_init();
	timmer_init();
	sched_init();
	local_timer_init();
	// fb_init();
	// fb_show();
}
void shell()
{
	char buf[1024];
	while (1) {
		print("njt@osdi2020# ");
		readline(buf);
		if (*buf) {
			print("\n");
			cmd(buf);
		} else {
			print("\n");
		}
		// sched_next();
	}
}
int32_t fork()
{
	asm volatile("mov x0, #11\n\t"
		     "svc #0");
	struct task_t *current = &task_pool[current_task];
	return current->trapframe;
}
void foo1()
{
	// print("PID: %d\n", fork());
	// int cnt = 0;
	// int32_t pid = fork();
	// if (pid == -1) {
	// 	print("Fork error\n");
	// 	return;
	// } else if (pid == 0) {
	// 	/* child */
	// 	print("child cnt %x\n", &cnt);
	// } else {
	// 	print("parent cnt %x\n", &cnt);
	// }
	while (1) {
		print("Task id: %d ", task_pool[current_task].task_id);
		print("with level %d\n", task_pool[current_task].privilege);
		delay(1000000);
		// sched_next();
	}
}
void user_test()
{
	do_exec(foo1, 0);
}
void jmp_to_el0();
int main()
{
	init();
	// privilege_task_create(idle);
	// for (int i = 0; i < 2; i++) {
	privilege_task_create(foo);
	// }
	privilege_task_create(user_test);
	// jmp_to_el0();
	idle();
	// shell();
	return 0;
}
