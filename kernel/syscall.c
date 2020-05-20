#include "kernel/exception.h"
#include "kernel/mini_uart.h"
#include "kernel/mm.h"
#include "kernel/sched.h"
#include "kernel/lib/string.h"
#include "kernel/syscall.h"

uint32_t do_get_taskid(void) {
  return get_current_task()->id;
}

size_t do_uart_read(void *buf, size_t count) {
//  mutex_lock(&uart_lock);
  enable_interrupt();
  mini_uart_getn(/* verbose */ true, buf, count);
  disable_interrupt();
//  mutex_unlock(&uart_lock);
  /* For now, just assume all "count" bytes can be read. */
  return count;
}

size_t do_uart_write(const void *buf, size_t count) {
//  mutex_lock(&uart_lock);
  mini_uart_putn(buf, count);
//  mutex_unlock(&uart_lock);
  /* For now, just assume all "count" bytes can be written. */
  return count;
}

void do_exec(uint64_t binary_start, size_t binary_size) {
  uint64_t *pgd = build_user_va(binary_start, binary_size);
  create_mapping(pgd, USER_STACK_VA_TOP);
  asm volatile("msr ttbr0_el1, %0" : : "r"(pgd));
  el1_to_el0(0, (uint8_t *)USER_STACK_VA_BASE);
}

int do_fork_helper(struct trapframe *tf, uint64_t lr) {
  uint32_t cid = privilege_task_init();
  uint32_t pid = do_get_taskid();

  memcpy(kstack_pool[cid], kstack_pool[pid], MAX_STACK_SIZE);

  uint64_t *ppgd, *cpgd = (uint64_t *)PA_TO_KVA(page_alloc());
  asm volatile("mrs %0, ttbr0_el1" : "=r"(ppgd));
  task_pool[cid].context.ttbr0 = (uint64_t)cpgd;
  copy_vmmap(cpgd, ppgd, 1);

  task_pool[cid].context.x19 = lr;
  task_pool[cid].context.lr = (uint64_t)post_fork_child_hook;
  task_pool[cid].context.sp = (uint64_t)kstack_pool[cid] + ((uint64_t)tf - (uint64_t)kstack_pool[pid]);

  enqueue(&runqueue, &task_pool[cid]);
  return cid;
}

void do_exit(int status) {
  uint64_t *pgd;
  asm volatile("mrs %0, ttbr0_el1" : "=r"(pgd));
  reclaim_vmmap(pgd, 1);

  get_current_task()->state = TASK_ZOMBIE;
  get_current_task()->exit_status = status;
  schedule();
}

int do_kill(uint32_t id, int sig) {
  task_pool[id].sig_pending[sig] = true;
}

uint64_t do_get_remain_page_num(void) {
  return free_page_nums;
}

void *syscall_table[] = {
  do_get_taskid,
  do_uart_read,
  do_uart_write,
  do_exec,
  do_fork,
  do_exit,
  do_kill,
  do_get_remain_page_num
};
