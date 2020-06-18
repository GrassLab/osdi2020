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
	char name[20] = "tmpfs1";
	struct filesystem *tmpfs_1;
	setup_tmpfs(&tmpfs_1, name);
	register_filesystem(tmpfs_1, "tmpfs");
	init_root_filesystem();
	
	struct file* a = vfs_open("hello", REG_FILE | O_CREAT);
	struct file* b = vfs_open("world", REG_FILE | O_CREAT); 
	vfs_write(a, "Hello ", 6);
	vfs_write(b, "World!", 6);
	vfs_close(a);
	vfs_close(b);
	b = vfs_open("hello", REG_FILE);
	a = vfs_open("world", REG_FILE);
	int sz;
	char buf[100];
	sz  = vfs_read(b, buf, 100);
	sz += vfs_read(a, buf + sz, 100);
	buf[sz] = '\0';
	vfs_close(a);
	vfs_close(b);
	printf("%s\r\n", buf);
	
	vfs_ls("/", REG_DIR);
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
