#include "command.h"
#include "irq.h"
#include "lfb.h"
#include "mbox.h"
#include "mm.h"
#include "obj_alloc.h"
#include "schedule.h"
#include "string.h"
#include "timer.h"
#include "uart.h"
#include "vfs.h"

void vfs_test()
{
	struct file *a = vfs_open("hello", 0);
	a = vfs_open("hello", O_CREAT);
	vfs_close(a);
	struct file *b = vfs_open("hello", 0);
	vfs_close(b);
}

void alloc_test()
{
	printf("page addr = %x\n", &page);
	int i = fix_obj_alloc();
	char *get1 = (char *)fix_object_array[i].obj;
	printf("get obj = %x , index = %d\n", get1, i);
	i = fix_obj_alloc();
	char *get2 = (char *)fix_object_array[i].obj;
	printf("get obj = %x , index = %d\n", get2, i);
	i = fix_obj_alloc();
	char *get3 = (char *)fix_object_array[i].obj;
	printf("get obj = %x , index = %d\n", get3, i);

	obj_free(1);

	i = fix_obj_alloc();
	char *get4 = (char *)fix_object_array[i].obj;
	printf("get obj = %x , index = %d\n", get4, i);
}

void v_alloc_test()
{
	int i = varied_size_alloc(8000);
	char *get1 = (char *)fix_object_array[i].obj;
	printf("get obj = %x , index = %d\n", get1, i);
	obj_free(i);
}

void buddy_test()
{
	printf("frame addr = %x\n", &page);
	printf("page_use addr = %x\n", &page_use);
	unsigned long addr1 = page_alloc(1 * page_size);
	printf("addr = %x\n", addr1);
	printf("==================\n");
	unsigned long addr2 = page_alloc(1 * page_size);
	printf("addr = %x\n", addr2);
	printf("==================\n");
	page_free(addr1);
	printf("==================\n");
	page_free(addr2);
	printf("==================\n");
	addr1 = page_alloc(2 * page_size);
}

void testfun()
{
	while (1)
	{
		uart_puts("exec...\n");
		wait_cycles(100000);
	}
}

char *welcome = " \
░░░░░░░░░░░░░░░░▄░█▄░█▄▄▄░░\n \
█▀▀█░░█▀▀█░█▀▀█▄█▄█▄░█▄▄▄█░\n \
█▀▀█░░█▀▀█░█░░█░░▄█▄▄▄░░░░░\n \
█▀▀▀░░▀▀▀█░█░░█░▀▄▄▄█▄▄▄░░░\n \
█░▄▄▄▄█▄░█░▀▀▀▀░░█▄▄█▄▄█░░░\n \
█░░░▄▀█░░█░░░░░░░█▄▄█▄▄█░░░\n \
█░▄▀░▄█░▄█░░░░░░▄▀░░░░▄█░░░\n \
";

void compair(char *buf)
{
	if (strcmp(buf, "hello"))
	{
		printf("Hello World!\n");
	}
	else if (strcmp(buf, "help"))
	{
		printf("help : print all  command\n");
		printf("hello : say hello\n");
		printf("timestamp : get time\n");
		printf("reboot : reboot rpi3 \n");
		printf("loadimg : loadimg you want \n");
		printf("version : show board revision \n");
		printf("vc_addr : show vc base address \n");
	}
	else if (strcmp(buf, "timestamp"))
	{
		asm volatile("mov x0, #1");
		asm volatile("svc #0");
	}
	else if (strcmp(buf, "reboot"))
	{
		reboot();
	}
	else if (strcmp(buf, "loadimg"))
	{
		loadimg();
	}
	else if (strcmp(buf, "version"))
	{
		board_revision();
	}
	else if (strcmp(buf, "exec"))
	{
		execpt();
	}
	else if (strcmp(buf, "vc_addr"))
	{
		vc_base_address();
	}
	else if (strcmp(buf, "irq"))
	{
		irq();
	}
	else if (strcmp(buf, "lv"))
	{
		ellevel();
	}
	else if (strcmp(buf, "test1"))
	{
		test1();
	}
	else if (strcmp(buf, "test2"))
	{
		test2();
	}
	else if (strcmp(buf, "test3"))
	{
		test3();
	}
	else if (strcmp(buf, "buddy"))
	{
		buddy_test();
	}
	else if (strcmp(buf, "alloc"))
	{
		alloc_test();
	}
	else if (strcmp(buf, "valloc"))
	{
		v_alloc_test();
	}
	else if (strcmp(buf, "vfs"))
	{
		vfs_test();
	}
	else
	{
		printf("unknow command %s\n", buf);
	}
	return;
}
int main()
{
	uart_init();
	init_buddy();
	rootfs_init();

	// wait_cycles(10000);
	// lfb_init();
	// wait_cycles(10000);
	// // display a pixmap
	// lfb_showpicture();
	// wait_cycles(10000);

	uart_puts(welcome);
	printf("# ");

	char commandbuf[100];
	clearbuf(commandbuf, 100);
	int count = 0;
	while (1)
	{
		char get = uart_getc();
		uart_send(get);
		if (get != '\n')
		{
			commandbuf[count] = get;
			count++;
		}
		else
		{
			commandbuf[count] = '\x00';
			compair(commandbuf);
			printf("# ");
			count = 0;
			clearbuf(commandbuf, 100);
		}
	}
}
