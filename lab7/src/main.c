#include "uart.h"
#include "sched.h"
#include "fork.h"
#include "buddy.h"
#include "sys.h"
#include "vfs.h"
//required 1 & required 2
/*
void foo(){
  while(1) {
    uart_puts("Task id: ");
    uart_hex(current -> id);
    uart_puts("\n");
    delay(1000000);
    Schedule();
  }
}

void idle(){
  while(1){
    Schedule();
    delay(1000000);
  }
}

void main() {
  // ...
  // boot setup
  // ...
  uart_init();
  uart_getc();
  uart_puts("MACHINE IS OPEN!!\n");
  
  init_page_map();
  
  unsigned long current_el;
  current_el = get_el();
  uart_puts("Current EL: ");
  uart_hex(current_el);
  
  int N=5;
  for(int i = 0; i < N; ++i) { // N should > 2
    privilege_task_create(PF_KTHREAD, (unsigned long)&foo, 0);
  }

  idle();
}

*/

void uart_test(){
    char buffer[16]="writewrite\n";
    buffer[12]='\0';  
    
    int size;
    size = uart_write(buffer,sizeof(buffer));
    uart_puts("size: 0x"); 
    uart_hex(size);
    uart_puts("\n"); 
   
    size = uart_read(buffer,sizeof(buffer));
    uart_puts("size: 0x"); 
    uart_hex(size);
    uart_puts("\n"); 
}

void foo(){
  int tmp = 5;

  uart_puts("Task ");
  uart_hex(get_taskid());
  uart_puts(" after exec, tmp address 0x");
  uart_hex(&tmp);
  uart_puts(", tmp value ");
  uart_hex(tmp);
  uart_puts("\n");
  
  //uart_test();  

  exit(0);
  uart_puts("Should not be printed\n");
}

void test2(){
    unsigned long token = allocator_register(0x90);
    void *ptr1 = allocator_alloc(token);
    uart_puts("Get address ");
    uart_hex(ptr1);
    uart_puts("\n");

    void *ptr2 = malloc(0x50);
    uart_puts("Get address ");
    uart_hex(ptr2);
    uart_puts("\n");
    
    allocator_free(token, ptr1);   
    free(ptr2);   
    uart_getc() ;
    
    exit();
}


void test1(){
    
    unsigned long token = allocator_register(0x60);

    void *ptr1 = allocator_alloc(token);
    uart_puts("Get address ");
    uart_hex(ptr1);
    uart_puts("\n");
    
    void *ptr2 = allocator_alloc(token);
    uart_puts("Get address ");
    uart_hex(ptr2);
    uart_puts("\n");

    allocator_free(token, ptr1);
    allocator_free(token, ptr2);

    void *ptr3 = allocator_alloc(token);
    uart_puts("Get address ");
    uart_hex(ptr3);
    uart_puts("\n");
    allocator_free(token, ptr3);
    
    uart_getc() ;

    test2();
}


void test() {
  int cnt = 1;
  if (fork() == 0) {
    delay(100000);
    while(cnt < 10) {
      uart_puts("Task id: ");
      uart_hex(get_taskid());
      uart_puts(", cnt: ");
      uart_hex(cnt);
      uart_puts("\n");

      delay(10000);
      ++cnt;
    }
    exit(0);
    uart_puts("Should not be printed\n");
  } else {
    uart_puts("Task ");
    uart_hex(get_taskid());
    uart_puts(" before exec, cnt address 0x");
    uart_hex(&cnt);
    uart_puts(", cnt value ");
    uart_hex(cnt);
    uart_puts("\n");
  
    exec(test1);
  }
}

void test_lab7_req2(){
    uart_puts("start test req2\r\n");
    struct file* a = vfs_open("hello", 0);
    if(a == NULL){
        uart_puts("pass, $vfs_open(hello, 0) because file hello does not exist!\r\n");
    }
    
    a = vfs_open("hello", O_CREAT);
    if(a != NULL){
        uart_puts("pass, $vfs_open(hello, O_CREAT)\r\n");
    }
    vfs_close(a);

    struct file* b = vfs_open("hello", 0);
    if(b != NULL){
        uart_puts("pass, $vfs_open(hello, 0) because file hello exists now!\r\n");
    }
    vfs_close(b);
}

void test_lab7_req3(){
    uart_puts("start test req3\r\n");
    struct file* a = vfs_open("hello", O_CREAT); // file already exist
    struct file* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    char buf[250];
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf); // should be Hello World!
    uart_getc();
    uart_puts("\r\n"); 
}

void test_lab7_req4(){
    uart_puts("start test write\r\n");
    struct file* a = vfs_open("hello1", O_CREAT);
    struct file* b = vfs_open("world1", O_CREAT);
    vfs_write(a, "01234 ", 6);
    vfs_write(b, "56789!", 6);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello1", 0);
    a = vfs_open("world1", 0);
    int sz;
    char buf[250];
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_puts(buf); // should be 01234 Hello 56789!World!
    uart_puts("\r\n"); 
    uart_getc();
}

void test_lab7_req5(){
    uart_puts("start test read\r\n");
    struct file* a = vfs_open("hello1", 0);
    struct file* b = vfs_open("world1", 0);
    char buf[250];
    int sz;
    sz = vfs_read(a, buf, 6);
    sz += vfs_read(b, buf + sz, 6);
    sz += vfs_read(a, buf + sz, 6);
    sz += vfs_read(b, buf + sz, 6);
    buf[sz] = '\0';
    uart_puts(buf); // should be 01234 Hello 56789!World!
    uart_puts("\r\n"); 
    uart_getc();
}
 
void test_lab7_elc1(){
    uart_puts("start test elctive1\r\n");
    // create some regular files at root directory
    struct file* a = vfs_open("file1", O_CREAT);
    struct file* b = vfs_open("file2", O_CREAT);
    struct file* c = vfs_open("file3", O_CREAT);
    struct file* d = vfs_open("file4", O_CREAT);

    struct file* root = vfs_open("/", 0);

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
    test_lab7_req2();
    test_lab7_req3();
    test_lab7_req4();
    test_lab7_req5();
    test_lab7_elc1();
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

