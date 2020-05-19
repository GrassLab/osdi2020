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
  mini_uart_getn(/* verbose */ true, buf, count);
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

//int do_fork_helper(uint64_t trapframe, uint64_t lr) {
//  uint64_t pksp = trapframe;
//  uint64_t pusp = *((uint64_t *)(trapframe + 8 * 31));
//  uint64_t pfp = *((uint64_t *)(trapframe + 8 * 29));
//
//  uint32_t cid = privilege_task_init();
//  uint32_t pid = do_get_taskid();
//
//  memcpy(kstack_pool[cid], kstack_pool[pid], MAX_STACK_SIZE);
//  memcpy(ustack_pool[cid], ustack_pool[pid], MAX_STACK_SIZE);
//  memcpy(&task_pool[cid].context, &task_pool[pid].context, sizeof(struct task_context));
//
//  uint64_t cksp = (uint64_t)kstack_pool[cid + 1] - ((uint64_t)kstack_pool[pid + 1] - pksp);
//  uint64_t cusp = (uint64_t)ustack_pool[cid + 1] - ((uint64_t)ustack_pool[pid + 1] - pusp);
//  uint64_t cfp = (uint64_t)ustack_pool[cid + 1] - ((uint64_t)ustack_pool[pid + 1] - pfp);
//
//  task_pool[cid].context.x19 = lr;
//  task_pool[cid].context.lr = (uint64_t)post_fork_child_hook;
//  task_pool[cid].context.sp = cksp;
//
//  uint64_t child_trapframe = (uint64_t)kstack_pool[cid + 1] - ((uint64_t)kstack_pool[pid + 1] - trapframe);
//  *((uint64_t *)(child_trapframe + 8 * 31)) = cusp;
//  /*
//   * Update child's frame pointer explicitly in case that parent already
//   * used it to access local variables.
//   */
//  *((uint64_t *)(child_trapframe + 8 * 29)) = cfp;
//  enqueue(&runqueue, &task_pool[cid]);
//
//  return cid;
//}

void do_exit(int status) {
  get_current_task()->state = TASK_ZOMBIE;
  get_current_task()->exit_status = status;
  schedule();
}

int do_kill(uint32_t id, int sig) {
  task_pool[id].sig_pending[sig] = true;
}

void *syscall_table[] = {
  do_get_taskid,
  do_uart_read,
  do_uart_write,
  do_exec,
//  do_fork,
  NULL,
  do_exit,
  do_kill
};
