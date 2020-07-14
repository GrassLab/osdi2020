#include "uart.h"
#include "sched.h"
#include "fork.h"
#include "buddy.h"
#include "sys.h"
#include "vfs.h"

void test(){
    uart_puts("Test open!\r\n");
    struct file *a, *b;
    a = vfs_open("hello", 0);
    if(a == NULL){
        uart_puts("pass, vfs_open(hello, 0) because file hello does not exist!\r\n");
    }

    a = vfs_open("a.txt", 0);
    if(a != NULL){
        uart_puts("pass, vfs_open(a.txt, 0) because file a.txt exists!\r\n");
    }

    uart_puts("Test read!\r\n");
    char buf[512];
    int sz;
    sz = vfs_read(a, buf, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\r\n"); 
    uart_getc();
    vfs_close(a);
    
    uart_puts("Test write!\r\n");
    b = vfs_open("b.txt", 0);
    if(b != NULL){
        uart_puts("pass, vfs_open(b.txt, 0) because file b.txt exists!\r\n");
    }
    sz = vfs_read(b, buf, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\r\n"); 
    uart_getc();
 
    sz = vfs_write(b, "world~hello~", 12);
    vfs_close(b);
    
    b = vfs_open("b.txt", 0);
    sz = vfs_read(b, buf, 100);
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\r\n");
    uart_getc();
    vfs_close(b);
}

void zombie_reaper(){
    while(1){
        uart_puts("I'm zombie_reaper\n");
        struct task* p;
        for (int i = 0; i < NR_TASKS; i++){
            p = task_pool[i];
            if(p && p->state == TASK_ZOMBIE){
                uart_puts("free ");
                uart_hex(i);
                uart_puts("'s kernel stack and task struct.\n");
                free_page((unsigned long)p);
                task_pool[i] = 0;
            }
        } 
        Schedule();
        delay(100000);
    }
}

// -----------above is user code-------------
// -----------below is kernel code-------------

void user_test(){
  do_exec((unsigned long)&test);
}

void idle(){
  while(1){
    if(exist == 2) {
      break;
    }
    
    Schedule();
    delay(1000000);
  }
  
  Schedule();
  uart_puts("Test finished\n");
  disable_irq();
  while(1);
}

void main(void){

	uart_init();
    uart_getc();
    uart_puts("MACHINE IS OPEN!!\n");
    init_page_map();
    unsigned long current_el;
    current_el = get_el();
    uart_puts("Current EL: ");
    uart_hex(current_el);
    uart_puts("\n");

    init_rootfs();
    test();
/* 
    int res;
    res = privilege_task_create(PF_KTHREAD, (unsigned long)&zombie_reaper, 0);
    if (res < 0) {
        uart_puts("error while starting kernel process");
        return;
    }
    res = privilege_task_create(PF_KTHREAD, (unsigned long)&user_test, 0);
    if (res < 0) {
        uart_puts("error while starting kernel process");
        return;
    }
    core_timer_enable();
    enable_irq();
    
    idle();
*/    
//    shell();
}

