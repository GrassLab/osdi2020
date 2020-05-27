#include <stddef.h>
#include <stdint.h>

#include "buddy.h"
#include "fork.h"
#include "irq.h"
#include "kernel.h"
#include "list.h"
#include "mini_uart.h"
#include "mm.h"
#include "printf.h"
#include "sched.h"
#include "sys.h"
#include "timer.h"
#include "user.h"
#include "utils.h"

#define USER_PROCESS_BINARY 1

void kernel_process() {
  printf("Kernel process started. EL %d\r\n", get_el());

  while (1) {
  }

  /* #ifdef USER_PROCESS_BINARY */
  /*   unsigned long begin = (unsigned long)&_binary_user_img_start; */
  /*   unsigned long end = (unsigned long)&_binary_user_img_end; */
  /*   // exit_process(); */
  /*   int err = do_exec(begin, end - begin, 0x1000); */
  /* #else */
  /*   unsigned long begin = (unsigned long)&user_begin; */
  /*   unsigned long end = (unsigned long)&user_end; */
  /*   unsigned long process = (unsigned long)&user_process; */
  /*   int err = move_to_user_mode(begin, end - begin, process - begin); */
  /* #endif */

  /*   if (err < 0) { */
  /*     printf("Error while moving process to user mode\n\r"); */
  /*   } */
}

void zombie_reaper() {
  struct task_struct *p;
  while (1) {
    preempt_disable();
    for (int i = 0; i < NR_TASKS; ++i) {
      p = task[i];
      /* zombie reaper */
      if (p && p->state == TASK_ZOMBIE && p != current) {
        println("[Zombie] reap the zombie task %d", p->pid);
        /* free the kernel page */
        println("[Zombie] kernel page count %d", p->mm.kernel_pages_count);
        for (int i = 0; i < p->mm.kernel_pages_count; ++i) {
          free_page(p->mm.kernel_pages[i]);
        }

        /* free the task */
        free_page(va2phys((unsigned long)p));

        task[p->pid] = 0;
        nr_tasks--;
      }
    }
    preempt_enable();
    schedule();
  }
}

void kernel_main() {
  uart_init();
  init_printf(NULL, putc);

  println("LOW memory: %x", LOW_MEMORY);
  println("HIGH memory: %x", HIGH_MEMORY);

  /* Init the buddy system first with the base memory address */
  struct buddy *bd = Buddy.new(128, LOW_MEMORY);
  if (!bd) {
    printf("erorr while constructing the buddy system");
    return;
  }
  Buddy.show(bd);

  {
    /* allocate 32 */
    struct Pair p = Buddy.alloc(bd, 32);
    printf("alloc [%x to %x] w/ pair {%d, %d}\n", p.lb, p.ub, p.lb >> 12,
           p.ub >> 12);
    Buddy.show(bd);
  }

  {
    /* allocate 7 */
    struct Pair p = Buddy.alloc(bd, 7);
    printf("alloc [%x to %x] w/ pair {%d, %d}\n", p.lb, p.ub, p.lb >> 12,
           p.ub >> 12);
    Buddy.show(bd);
  }

  {
    /* allocate 64 */
    struct Pair p = Buddy.alloc(bd, 64);
    printf("alloc [%x to %x] w/ pair {%d, %d}\n", p.lb, p.ub, p.lb >> 12,
           p.ub >> 12);
    Buddy.show(bd);
  }


  /* { */
  /*   /\* allocate 56 *\/ */
  /*   struct Pair p = Buddy.alloc(bd, 56); */
  /*   printf("alloc [%x to %x] w/ pair {%d, %d}\n", p.lb, p.ub, p.lb >> 12, */
  /*          p.ub >> 12); */
  /*   Buddy.show(bd); */
  /* } */

  sys_core_timer_enable();

  /* int res; */

  /* res = copy_process(PF_KTHREAD, (unsigned long)&zombie_reaper, 0); */
  /* if (res < 0) { */
  /*   printf("error while starting zombie reaper"); */
  /*   return; */
  /* } */

  /* res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0); */
  /* if (res < 0) { */
  /*   printf("error while starting kernel process"); */
  /*   return; */
  /* } */

  while (1) {
    schedule();
  }
}
