#include <stdint.h>
#include "task.h"
#include "queue.h"
#include "schedule.h"
#include "uart.h"
#include "irq.h"
#include "string_util.h"
#include "syscall.h"

struct task_struct kernel_task_pool[TASK_POOL_SIZE];
uint16_t task_kernel_stack_pool[TASK_POOL_SIZE][TASK_KERNEL_STACK_SIZE];
uint16_t task_user_stack_pool[TASK_POOL_SIZE][TASK_USER_STACK_SIZE];

void task_privilege_task_create(void(*start_func)())
{
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
    return;
  }

  /* assign id */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].id = new_id;

  /* init quantum_count to 0 */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].quantum_count = 0;

  /* reset flag */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].flag = 0;

  /* assign context */
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.lr = (uint64_t)start_func;
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.fp = (uint64_t)(task_kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);
  kernel_task_pool[TASK_ID_TO_IDX(new_id)].cpu_context.sp = (uint64_t)(task_kernel_stack_pool[TASK_ID_TO_IDX(new_id)] + TASK_KERNEL_STACK_SIZE);

  /* push into queue */
  schedule_enqueue(new_id);
  return;
}

uint64_t task_get_current_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el1\n":
               "=r"(current_task_id));
  return current_task_id;
}

void task_idle(void)
{
  while(1)
  {
    while(schedule_check_queue_empty())
    {
      asm volatile ("wfi");
      /* keep waiting for new task even wake from timer interrupt */
    }
    scheduler();
  }
}

void task_privilege_demo(void)
{
  while(1)
  {
    char string_buff[0x10];
    uint64_t current_task_id = task_get_current_task_id();
    uint64_t current_quantum_count;
    uart_puts("Hi I'm privilege task id ");
    string_longlong_to_char(string_buff, (int64_t)current_task_id);
    uart_puts(string_buff);
    uart_putc('\n');
    irq_int_enable();
    uart_puts("Enabling timer interrupt\n");

    uart_puts("Waiting to reschedule\n");
    /* if quantum_count get from task_pool is less than previous one, the reschedule occurred */
    do
    {
      current_quantum_count = kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].quantum_count;
      asm volatile("wfi");
    }
    while(current_quantum_count <= kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].quantum_count);
  }
}

void task_do_exec(void(*start_func)())
{
  uint64_t current_task_id = task_get_current_task_id();
  /* setup register and eret */
  asm volatile(
    "mov x0, %0\n"
    "msr sp_el0, x0\n"
    : : "r"((uint64_t)(task_user_stack_pool[TASK_ID_TO_IDX(current_task_id)] + TASK_USER_STACK_SIZE)));

  asm volatile(
    "mov x0, %0\n"
    "msr elr_el1, x0\n"
    : : "r"(start_func));

  asm volatile(
    "eor x0, x0, x0\n"
    "msr spsr_el1, x0\n"
    "eret");
}

void task_user_demo(void)
{
  char string_buff[0x10];
  uint64_t current_task_id = task_get_current_task_id();

  irq_int_enable();
  uart_puts("task_user_demo in kernel mode with irq enable id ");
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(string_buff);
  uart_putc('\n');

  task_do_exec(task_user_context_demo);
}

void task_user_context_demo(void)
{
  char string[] = "task_user_demo in user mode. Type and echo\n";
  syscall_uart_puts(string);
  char input_string[0x20];
  while(1)
  {
    syscall_uart_gets(input_string, '\n', 0x20 - 2);
    syscall_uart_puts(input_string);
  }
}

