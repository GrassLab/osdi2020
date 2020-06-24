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
#include "vfs.h"
#include "tmpfs.h"
#include "sdhost.h"
#include "fat32.h"

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
	sd_init();
	char name[20] = "fat32_fs1";
	struct filesystem *fat32_1;
	setup_fat32(&fat32_1, name);
	register_filesystem(fat32_1, "fat32_fs");
	init_root_filesystem();
	struct file *a = vfs_open("hello", REG_FILE);
	if (a != 0) {
		vfs_write(a, "hello world", 11);
		vfs_close(a);
	}
	a = vfs_open("hello", REG_FILE);
	if (a != 0) {
		int sz;
		char buf[100];
		sz  = vfs_read(a, buf, 100);
		printf("%s\r\n", buf);
	}
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
