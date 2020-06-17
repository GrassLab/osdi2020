#include "kernel/exception.h"
#include "kernel/lib/ioutil.h"
#include "kernel/mini_uart.h"
#include "kernel/mm.h"
#include "kernel/sched.h"
#include "kernel/shell.h"
#include "kernel/syscall.h"
#include "kernel/lib/types.h"
#include "kernel/vfs.h"
#include "kernel/tmpfs.h"

#define assert(cond)                                                    \
  do {                                                                  \
    if (!(cond)) {                                                      \
      printk("Assertion Fail: %s:%u: %s\n", __FILE__, __LINE__, #cond); \
      for (;;) {}                                                       \
    }                                                                   \
  } while (0)

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

  char buf[128];
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

  /* Filesystem initialization */
  rootfs = (struct mount *)buddy_alloc(0);
  tmpfs_setup_mount(&tmpfs, rootfs);

  /* Test cases for vfs_open() and vfs_close() */
  struct file* a = vfs_open("hello", 0);
  assert(a == NULL);
  a = vfs_open("hello", O_CREAT);
  assert(a != NULL);
  vfs_close(a);
  struct file* b = vfs_open("hello", 0);
  assert(b != NULL);
  vfs_close(b);

  /* Test cases for vfs_read() and vfs_write() */
  a = vfs_open("hello", O_CREAT);
  b = vfs_open("world", O_CREAT);
  vfs_write(a, "Hello ", 6);
  vfs_write(b, "World!", 6);
  vfs_close(a);
  vfs_close(b);
  b = vfs_open("hello", 0);
  a = vfs_open("world", 0);
  int sz;
  sz = vfs_read(b, buf, 100);
  sz += vfs_read(a, buf + sz, 100);
  buf[sz] = '\0';
  printk("%s\n", buf); // should be Hello World!
  vfs_close(a);
  vfs_close(b);

  /* Test cases for vfs_readdir() */
  a = vfs_open("/", O_CREAT);
  while (vfs_readdir(a, buf, sizeof(buf)) == 0) {
    printk("/%s\n", buf);
  }
  vfs_close(a);

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
