#include <stdint.h>
#include "task.h"
#include "schedule.h"
#include "uart.h"
#include "irq.h"
#include "string_util.h"
#include "syscall.h"
#include "sys.h"
#include "shell.h"
#include "signal.h"
#include "mmu.h"
#include "slab.h"

struct task_struct * kernel_task_pool;
uint16_t * task_kernel_stack_pool;

void task_init()
{
  uart_puts("Task init started\n");
  kernel_task_pool = (struct task_struct *)slab_malloc(sizeof(struct task_struct) * TASK_POOL_SIZE);
  task_kernel_stack_pool = (uint16_t *)slab_malloc(sizeof(uint16_t) * TASK_POOL_SIZE * TASK_KERNEL_STACK_SIZE);
  uart_puts("Task init complete\n");
  return;
}

uint64_t task_privilege_task_create(void(*start_func)(), unsigned priority)
{
  task_guard_section();
  unsigned new_id = 0;

  /* find usable position in task_pool */
  for(unsigned idx = 0; idx < TASK_POOL_SIZE; ++idx)
  {
    if(kernel_task_pool[idx].id != idx + 1)
    {
      new_id = idx + 1;
      break;
    }
  }
  if(new_id == 0)
  {
    /* failed to create new task */
    return 0;
  }

  /* assign id */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].id = new_id;

  /* init quantum_count to 0 */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].quantum_count = 0;

  /* reset flag */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].flag = 0;

  /* reset signal */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].signal = 0;

  /* set priority */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].priority = (uint64_t)priority;

  /* assign context */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.lr = (uint64_t)start_func;

  /* stack grow from high to low */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.fp = (uint64_t)(task_kernel_stack_pool + new_id * TASK_KERNEL_STACK_SIZE);
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.sp = (uint64_t)(task_kernel_stack_pool + new_id * TASK_KERNEL_STACK_SIZE);

  /* push into queue */
  schedule_enqueue(new_id, (unsigned)priority);
  task_guard_section();
  return new_id;
}

uint64_t task_get_current_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el1\n":
               "=r"(current_task_id));
  return current_task_id;
}

void task_shell(void)
{
  char ann[] = ANSI_BG_GREEN ANSI_BLACK"[Kernel mode shell]"ANSI_RESET" ";
  char string_buff[0x80];
  uint64_t current_task_id = task_get_current_task_id();

  uart_puts(ann);
  uart_puts("ID: ");
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(string_buff);
  uart_puts(" Entering user mode shell\n");
  uart_puts(ann);
  task_do_exec((uint64_t *)&_binary_shell_bin_start, (uint64_t)&_binary_shell_bin_end - (uint64_t)&_binary_shell_bin_start);
}

void task_do_exec(uint64_t * start, uint64_t size)
{
  char string_buff[0x40];
  uart_puts("Start: ");
  string_ulonglong_to_hex_char(string_buff, (unsigned long long)&start);
  uart_puts(string_buff);
  uart_puts("\tsize: ");
  string_ulonglong_to_hex_char(string_buff, (unsigned long long)size);
  uart_puts(string_buff);
  uart_puts("\n");
  /* CAUTION: kernel stack may explode if you keep doing exec */

  uint64_t current_task_id = task_get_current_task_id();
  struct user_space_mm_struct * current_user_mm_struct = &(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].user_space_mm);

  /* Ask scheduler to switch pmd because it use user space */
  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_USER_SPACE);

  /* create user space va */
  mmu_create_user_pmd_pte(current_user_mm_struct);

  /* setup pmc */
  mmu_user_task_set_pmd(current_user_mm_struct);

  /* copy memory to physical page frame */
  mmu_copy_user_to_text((char *) start, current_user_mm_struct, (unsigned)size);

  /* setup register and eret */
  asm volatile(
    "mov x0, %0\n"
    "msr sp_el0, x0\n"
    : : "r"(USER_STACK_VA)); /* stack grows from high to low */

  asm volatile(
    "mov x0, %0\n"
    "msr elr_el1, x0\n"
    : : "r"(0x0));

  asm volatile(
    "eor x0, x0, x0\n"
    "msr spsr_el1, x0\n"
    "eret");
}

uint64_t task_get_current_task_signal(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  return kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].signal;
}

void task_start_waiting(void)
{
  uint64_t current_task_id = task_get_current_task_id();

  task_guard_section();

  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 2);
  schedule_enqueue_wait(current_task_id);
  task_unguard_section();

  schedule_yield();
  return;
}

void task_end_waiting(void)
{
  /* put the first task in the wait queue back to running queue */
  uint64_t task_id = schedule_dequeue_wait();
  if(task_id == 0)
  {
    return;
  }

  /* Some task in wait queue might be zombie */
  while(CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(task_id)].flag, TASK_STATE_ZOMBIE))
  {
    task_id = schedule_dequeue_wait();
  }

  CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(task_id)].flag, 2);
  schedule_enqueue(task_id, (unsigned)kernel_task_pool[TASK_ID_TO_IDX(task_id)].priority);
  task_unguard_section();
  return;
}

void task_guard_section(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_GUARD);
  return;
}

void task_unguard_section(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_GUARD);
  return;
}

int task_is_guarded(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  return CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_GUARD);
}

