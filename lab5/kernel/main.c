#include "bh.h"
#include "fork.h"
#include "framebuffer.h"
#include "irq.h"
#include "libc.h"
#include "miniuart.h"
#include "sched.h"
#include "shell.h"
#include "sys.h"
#include "mm.h"
#include "timer.h"

int get_el() {
  int el;
  asm volatile("mrs %0, CurrentEL\n"
               "lsr %0, %0, #2"
               : "=r"(el));
  return el;
}

void user_process1(char *array) {

  char buf[2] = {0};
  while (1) {
    /* call_sys_write("User prcoess 1\r\n"); */
    for (int i = 0; i < 5; i++) {
      buf[0] = array[i];
      call_sys_write(buf);
      delay(100000);
    }
  }
}

void program() {
  call_sys_write("program started\r\n");
  while (1) {
    delay(100000);
  }
  call_sys_exit();
}

void bar() {
  int tmp = 5;
  sys_println("Task %d after exec, tmp address 0x%x, tmp value %d\n",
              call_sys_getid(), &tmp, tmp);
  call_sys_exit();
}

void test_case() {
  int cnt = 1;
  if (call_sys_fork() == 0) {

    call_sys_fork();
    delay(10000000);
    call_sys_fork();

    while (cnt < 10) {

      sys_println("Task id: %d, cnt: %d", call_sys_getid(), cnt);
      delay(10000000);
      ++cnt;
    }
    call_sys_exit();
    sys_println("shoud not be printed");
  } else {

    sys_println("Task %d before exec, cnt address 0x%x, cnt value %d",
                call_sys_getid(), &cnt, cnt);
    call_sys_exec(bar);
  }
  call_sys_exit();
}

void init_process() {
  sys_println("shoud not be printed");
  /* tty */
  /* stat_memory_usage(); */


  /* int pid = call_sys_fork(); */
  /* if (pid == 0) { */
  /*   call_sys_exec(shell); */
  /* } */

  /* /\* test case *\/ */
  /* if (call_sys_fork() == 0) { */
  /*   call_sys_exec(test_case); */
  /* } */

  /* /\* exec the shell process *\/ */
  /* unsigned long stack = call_sys_malloc(); */
  /* if (stack < 0) { */
  /*   uart_println("Error while allocating stack for shell"); */
  /* } else { */
  /*   int err = call_sys_clone((unsigned long)&shell, 0, stack); */
  /*   if (err < 0) { */
  /*     uart_println("Error while clonning shell"); */
  /*   } */
  /* } */

  while (1) {
    delay(1000000);
  }
  call_sys_exit();
}


void kernel_process() {
  uart_println("Kernel process started. EL %d", get_el());
  uart_println("kernel proess run at %x%x",
               (unsigned long)&kernel_process >> 32,
               (unsigned long)&kernel_process);


  char c;
  while ((c = call_sys_read())) {
    uart_println("%c", c);
  }

  /* uart_println("Memory usage:"); */
  /* /\* stat_memory_usage(); *\/ */

  /* /\* do_exec(init_process); *\/ */
  /* /\* do_exec(init_process); *\/ */
  /* extern unsigned long user_begin; */
  /* extern unsigned long user_end; */
  /* extern unsigned long user_process; */
  /* unsigned long begin = (unsigned long)&user_begin; */
  /* unsigned long end = (unsigned long)&user_end; */
  /* unsigned long process = (unsigned long)&user_process; */
  /* uart_println("%x%x %x%x %x%x", begin >> 32, begin, end >> 32, end, process >> 32, process); */


  /* /\* int err = move_to_user_mode(begin, end - begin, process - begin); *\/ */
  /* int err = move_to_user_mode(0, 0, (unsigned long)init_process); */
  /* if (err < 0) { */
  /*   uart_println("Error while moving process to user mode\n\r"); */
  /* } */




  while (1) {

  }
}

void foo() {
  while (1) {
    /* uart_println("Task id: %d\n", current->pid); */
    delay(1000000);
  }
}

/* int ipc_index = 0; */

/* struct mail { */
/*   int type; */
/*   char *message; */
/* }; */

/* struct mail ipc_box[BUF_SIZE] = {}; */

void idle() {
  while (1) {
    /* maintain the ipc */

    /* if (nr_tasks == 0) */
    /*   break; /\* TODO *\/ */
    schedule();
  }
}

void el1_main() {
  uart_init();

  uart_println("Hello world");



  /* while ((c = uart_getc())) { */
  /*   uart_send(c); */
  /* } */
  /* { */
  /*   #include "gpio.h" */
  /*   #define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
   */
  /*   while (1) */
  /*     *AUX_MU_IO = 's'; */
  /* } */

  /* int res = copy_process(PF_KTHREAD, (unsigned long)&pm_daemon, 0); */
  /* if (res < 0) { */
  /*   uart_println("error while starting zombie reaper"); */
  /*   return; */
  /* } */
  /* shell(); */
  /* shell(); */
  /* privilege_task_create(kernel_process, 0); */
  // unsigned long page = allocate_kernel_page();
  /* page = allocate_kernel_page(); */


  copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
  /* copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0); */
  /* copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0); */
  /* unsigned long code_page = allocate_user_page(current, 10); */
  /* code_page = allocate_user_page(current, 20); */
  /* code_page = allocate_user_page(current, 0); */
  /* code_page = allocate_user_page(current, 0); */
  /* copy_process(PF_KTHREAD, (unsigned long)&shell, 0); */

  /* if (res < 0) { */
  /*   uart_println("error while starting kernel process"); */
  /*   return; */
  /* } */

  sys_core_timer_enable();
  /* /\* const int N = 10; *\/ */
  /* /\* for (int i = 0; i < N; ++i) { // N should > 2 *\/ */

  /* /\* } *\/ */
  idle();
}

int main(int argc, char *argv[]) {
  bh_mod_mask = 0;

  /* initialize uart with default clock & baud rate */
  uart_init();

  /* initialize framebuffer */
  lfb_init();

  /* start interactive shell */
  /* while (1); */
  shell();

  return 0;
}
