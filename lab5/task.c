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

struct task_struct kernel_task_pool[TASK_POOL_SIZE];
uint16_t task_kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE];

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
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.fp = (uint64_t)(task_kernel_stack_pool[new_id]);
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.sp = (uint64_t)(task_kernel_stack_pool[new_id]);

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

void task_test1(void)
{
  char ann[] = ANSI_BLUE"[Kernel test command 1] "ANSI_RESET;
  char string_buff[0x80];
  uint64_t current_task_id = task_get_current_task_id();

  uart_puts(ann);
  uart_puts("ID: ");
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(string_buff);
  uart_puts("\n");
  uart_puts(ann);
  task_do_exec((uint64_t *)&_binary_test1_bin_start, (uint64_t)&_binary_test1_bin_end - (uint64_t)&_binary_test1_bin_start);
}

void task_test2(void)
{
  char ann[] = ANSI_YELLOW"[Kernel test command 2] "ANSI_RESET;
  char string_buff[0x80];
  uint64_t current_task_id = task_get_current_task_id();

  uart_puts(ann);
  uart_puts("ID: ");
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(string_buff);
  uart_puts("\n");
  uart_puts(ann);
  task_do_exec((uint64_t *)&_binary_test2_bin_start, (uint64_t)&_binary_test2_bin_end - (uint64_t)&_binary_test2_bin_start);
}

void task_test3(void)
{
  char ann[] = ANSI_CYAN"[Kernel test command 3] "ANSI_RESET;
  char string_buff[0x80];
  uint64_t current_task_id = task_get_current_task_id();

  uart_puts(ann);
  uart_puts("ID: ");
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(string_buff);
  uart_puts("\n");
  uart_puts(ann);
  task_do_exec((uint64_t *)&_binary_test3_bin_start, (uint64_t)&_binary_test3_bin_end - (uint64_t)&_binary_test3_bin_start);
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

  /* setup pmc */
  uint64_t * user_page_frame_start = mmu_user_task_set_pmu(TASK_ID_TO_IDX(current_task_id));

  /* copy memory to physical page frame */
  memcopy((char *)start, (char *)user_page_frame_start, (unsigned)size);

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
  task_unguard_section();

  schedule_enqueue_wait(current_task_id);
  schedule_yield();
  return;
}

void task_end_waiting(void)
{
  /* put the first task in the wait queue back to running queue */
  uint64_t task_id = schedule_dequeue_wait();

  /* Some task in wait queue might be zombie */
  while(CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(task_id)].flag, TASK_STATE_ZOMBIE))
  {
    task_id = schedule_dequeue_wait();
  }

  task_guard_section();
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

