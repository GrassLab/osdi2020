#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "mm.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"
#include "slab.h"

extern unsigned long user_begin;
extern unsigned long user_end;
extern void user_process();
void kernel_process(){

	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	unsigned long begin = (unsigned long)&user_begin;
	unsigned long end = (unsigned long)&user_end;
	unsigned long process = (unsigned long)&user_process;
	int err = do_exec(begin, end - begin, process - begin); // virtual address of begin & size
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	} 
}


void kernel_main()
{
	uart_init();
	init_printf(NULL, putc);
	irq_vector_init();
	init_buddy_system();

	init_obj_allocator();

	// struct kmem_cache *kmem_cache_16 = kmem_cache_create(sizeof(int) * 2);
	// int *test = kmem_cache_alloc(kmem_cache_16);
	// test[0] = 1;
	// test[1] = 4;
	// printf("test[0] 0x%x: %d\r\n", &test[0], test[0]);
	// printf("test[1] 0x%x: %d\r\n", &test[1], test[1]);
	// slab_put_obj(test);

	int *test3 = obj_allocate(sizeof(int) * 4096);
	test3[4000] = 50;
	printf("test3[4000] 0x%x: %d\r\n", &test3[4000], test3[4000]);
	
	int *test4 = obj_allocate(sizeof(int) * 2);
	test4[0] = 1;
	test4[1] = 2;
	printf("test4[0] 0x%x: %d\r\n", &test4[0], test4[0]);
	printf("test4[1] 0x%x: %d\r\n", &test4[1], test4[1]);
	enable_core_timer();
	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	
	enable_irq();
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	while (1){
		free_zombie_task();
		schedule();
	}	
}
