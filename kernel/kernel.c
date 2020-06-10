#include "kernel/exception.h"
#include "kernel/lib/ioutil.h"
#include "kernel/mini_uart.h"
#include "kernel/mm.h"
#include "kernel/sched.h"
#include "kernel/shell.h"
#include "kernel/syscall.h"
#include "kernel/lib/types.h"

void delay(int t) {
  for (int i = 0; i < t; ++i) {}
}

void reaper(void) {
  while (true) {
    for (int i = 0; i < MAX_TASK_NUM; ++i) {
      if (task_inuse[i] == true && task_pool[i].state == TASK_ZOMBIE) {
        task_inuse[i] = false;
        printk("Task %u is reaped\n", i);
      }
    }
    schedule();
  }
}

void idle(void) {
  while (true) {
    schedule();
  }
}

extern char _binary_user_shell_img_start[];
extern char _binary_user_shell_img_size[];

void user_test(void) {
  do_exec((uint64_t)_binary_user_shell_img_start, (size_t)_binary_user_shell_img_size);
}

//void signal_test_receiver(void) {
//  while (true) {
//    printk("Task id: %u, Waiting for SIGKILL" EOL, do_get_taskid());
//    delay(100000);
//  }
//}
//
//void signal_test_sender(void) {
//  uint32_t receiver_id = privilege_task_create(signal_test_receiver);
//  do_kill(receiver_id, SIGKILL);
//  printk("Task id: %u, Created and sent SIGKILL to %u" EOL, do_get_taskid(), receiver_id);
//  do_exit(0);
//}

void print1(void) {
  while (true) {
    printk("1..." EOL);
    delay(1000000);
    schedule();
  }
}

void print2(void) {
  while (true) {
    printk("2..." EOL);
    delay(1000000);
    schedule();
  }
}

int main(void) {
  page_init();
  gpio_init();
  mini_uart_init();
  exception_init();
  asm("msr daifset, #0xf");
  core_timer_enable();
//  el1_to_el0();
//  shell();

  char buf[16];
  mini_uart_getn(true, buf, 2);
  buddy_init();
  for (int i = 0; i < MAX_ORDER; ++i) {
    printk("Order %u: %u\n", i, free_areas[i].nr_free);
    if (list_empty(&free_areas[i].free_list)) {
      continue;
    }
    for (struct list_head *iter = free_areas[i].free_list.next; iter != &free_areas[i].free_list; iter = iter->next) {
      printk("%#x\n", PFN_TO_KVIRT(list_entry(iter, struct page, free_list)->pfn));
    }
  }
  //struct page *a = buddy_alloc(1);
  // struct page *b = buddy_alloc(0);
  //buddy_free(a, 1);
  // buddy_free(b, 0);
  //for (;;) {}

  idle_task_create();
  privilege_task_create(reaper);
  privilege_task_create(shell);
//  privilege_task_create(print1);
//  privilege_task_create(print2);
  // for (int i = 0; i < 1; ++i) {
  //   privilege_task_create(user_test);
  // }
// 
// privilege_task_create(signal_test_sender);

  idle();
}
