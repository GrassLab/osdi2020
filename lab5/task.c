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

uint64_t task_user_get_current_task_id(void)
{
  uint64_t current_task_id;
  asm volatile("mrs %0, tpidr_el0\n":
               "=r"(current_task_id));
  return current_task_id;
}

void task_privilege_demo(void)
{
  char ann[] = ANSI_YELLOW"[Privilege task] "ANSI_RESET;
  int current_reschedule_count = 0;
  int max_reschedule_time = 3;

  while(1)
  {
    char string_buff[0x10];
    uint64_t current_task_id = task_get_current_task_id();
    uint64_t current_quantum_count;

    uart_puts(ann);
    uart_puts("Hi I'm id ");
    string_longlong_to_char(string_buff, (int64_t)current_task_id);
    uart_puts(string_buff);
    uart_putc('\n');
    irq_int_enable();
    uart_puts(ann);
    uart_puts("Enabling timer interrupt and waiting to reschedule\n");

    /* if quantum_count get from task_pool is less than previous one, the reschedule occurred */
    do
    {
      current_quantum_count = kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].quantum_count;
      asm volatile("wfi");
    }
    while(current_quantum_count <= kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].quantum_count);
    ++current_reschedule_count;
    if(current_reschedule_count >= max_reschedule_time)
    {
      uart_puts(ann);
      uart_puts("exiting\n");
      sys_exit(0);
    }
  }
}

void task_do_exec(uint64_t * start, uint64_t size)
{
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

void task_user_demo(void)
{
  char string_buff[0x10];
  uint64_t current_task_id = task_get_current_task_id();

  irq_int_enable();
  string_longlong_to_char(string_buff, (int64_t)current_task_id);
  uart_puts(ANSI_YELLOW"[Privilege task that will exec to user mode] "ANSI_RESET);
  uart_puts("In kernel mode with irq enable ID: ");
  uart_puts(string_buff);
  uart_putc('\n');

  task_do_exec(task_user_context1_demo);
}

void task_user_context1_demo(void)
{
  int demo_purpose_var_1 = 1;
  char string_buff[0x20];
  uint64_t task_id = task_user_get_current_task_id();

  syscall_uart_puts(ANSI_YELLOW"[Privilege task that exec\"ed\" to user mode] "ANSI_RESET);
  syscall_uart_puts("Task id: ");
  string_longlong_to_char(string_buff, (long)task_id);
  syscall_uart_puts(string_buff);
  syscall_uart_puts(" demo_purpose_var_1 address: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)&demo_purpose_var_1);
  syscall_uart_puts(string_buff);
  syscall_uart_puts(" demo_purpose_var_1 value: ");
  string_longlong_to_char(string_buff, demo_purpose_var_1);
  syscall_uart_puts(string_buff);

  syscall_uart_puts(ANSI_YELLOW"\n[Privilege task that exec\"ed\" to user mode] "ANSI_RESET"Let's exec in user mode\n");
  syscall_exec(task_user_context2_demo);
}

void task_user_context2_demo(void)
{
  int demo_purpose_var_2 = 3;
  char string_buff[0x80];
  int second_meeseek_id;
  uint64_t task_id = task_user_get_current_task_id();

  syscall_uart_puts(ANSI_BLUE"[I'm Mr.Meeseeks. Look at me] "ANSI_RESET);
  syscall_uart_puts("Task id: ");
  string_longlong_to_char(string_buff, (long)task_id);
  syscall_uart_puts(string_buff);
  syscall_uart_puts(" demo_purpose_var_2 address: ");
  string_ulonglong_to_hex_char(string_buff, (uint64_t)&demo_purpose_var_2);
  syscall_uart_puts(string_buff);
  syscall_uart_puts(" demo_purpose_var_2 value: ");
  string_longlong_to_char(string_buff, demo_purpose_var_2);
  syscall_uart_puts(string_buff);
  syscall_uart_puts(" Let's call another meeseeks.\n");

  second_meeseek_id = syscall_fork();
  if(second_meeseek_id == 0)
  {
    int third_meeseek_id;
    uint64_t current_sp;
    char current_sp_hex[0x10];
    char ann[0x80] = ANSI_MAGENTA"[I'm the second meeseeks] "ANSI_RESET;

    asm volatile("mov %0, sp" : "=r"(current_sp));
    string_ulonglong_to_hex_char(current_sp_hex, current_sp);
    syscall_uart_puts(ann);
    syscall_uart_puts("sp: ");
    syscall_uart_puts(current_sp_hex);
    syscall_uart_puts(" I fork a new meeseeks and exit when I recieve input\n");
    syscall_uart_gets(string_buff, '\n', 0x20 - 2);

    third_meeseek_id = syscall_fork();
    if(third_meeseek_id == 0)
    {
      int fourth_meeseek_id = syscall_fork();
      for(int i = 0; i < 100000; ++i);
      int fifth_meeseek_id = syscall_fork();

      if(fourth_meeseek_id == 0)
      {
        if(fifth_meeseek_id == 0)
        {
          ann[0] = '\0';
          string_concat(ann, ANSI_BLACK ANSI_BG_RED"[I'm the sixth meeseeks]"ANSI_RESET" ");
        }
        else
        {
          ann[0] = '\0';
          string_concat(ann, ANSI_BLACK ANSI_BG_GREEN"[I'm the fourth meeseeks]"ANSI_RESET" ");
        }
      }
      else
      {
        if(fifth_meeseek_id == 0)
        {
          ann[0] = '\0';
          string_concat(ann, ANSI_BLACK ANSI_BG_YELLOW"[I'm the fifth meeseeks]"ANSI_RESET" ");
        }
        else
        {
          ann[0] = '\0';
          string_concat(ann, ANSI_CYAN"[I'm the third meeseeks] "ANSI_RESET);
        }
      }

      while(demo_purpose_var_2 < 6)
      {
        syscall_uart_puts(ann);
        string_longlong_to_char(string_buff, demo_purpose_var_2);
        syscall_uart_puts("demo_purpose_var_2: ");
        syscall_uart_puts(string_buff);
        string_ulonglong_to_hex_char(string_buff, (uint64_t)&demo_purpose_var_2);
        syscall_uart_puts(" &demo_purpose_var_2: ");
        syscall_uart_puts(string_buff);
        syscall_uart_puts("\n");
        for(int i = 0; i < 100000; ++i);
        ++demo_purpose_var_2;
      }

      /* busy waiting until the first meeseeks kill us all */
      while(1);

    }
    else
    {
      syscall_uart_puts(ann);
      syscall_uart_puts("Owee new mission accomplished. [Poof]\n");
      syscall_exit(0);
    }
  }
  else
  {
    uint64_t current_sp;
    char current_sp_hex[0x10];
    char ann[] = ANSI_BLUE"[I'm the first meeseeks] "ANSI_RESET;

    asm volatile("mov %0, sp" : "=r"(current_sp));
    string_ulonglong_to_hex_char(current_sp_hex, current_sp);

    syscall_uart_puts(ann);
    syscall_uart_puts("sp: ");
    syscall_uart_puts(current_sp_hex);
    syscall_uart_puts(" New meeseeks has id of ");
    string_longlong_to_char(string_buff, second_meeseek_id);
    syscall_uart_puts(string_buff);
    syscall_uart_puts("\n");

    while(1)
    {
      asm volatile("mov %0, sp" : "=r"(current_sp));
      string_ulonglong_to_hex_char(current_sp_hex, current_sp);
      syscall_uart_puts(ann);
      syscall_uart_puts("sp: ");
      syscall_uart_puts(current_sp_hex);
      syscall_uart_puts(" I won't quit until you enter 's', press 'k' to kill all other meeseekses.\n");
      syscall_uart_gets(string_buff, '\n', 0x20 - 2);
      if(string_buff[0] == 's')
      {
        shell();
      }
      if(string_buff[0] == 'k')
      {
        /* In the demo scenario, the thrid meeseeks has task id 2 */
        syscall_uart_puts(ann);
        syscall_signal(3, SIGKILL);
        syscall_signal(5, SIGKILL);
        syscall_signal(6, SIGKILL);
        syscall_signal(7, SIGKILL);
        syscall_uart_puts("SIGKILL sent.\n");
      }
    }
  }
}

uint64_t task_get_current_task_signal(void)
{
  uint64_t current_task_id = task_get_current_task_id();
  return kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].signal;
}

void task_start_waiting(void)
{
  char id_char[0x10];
  uint64_t current_task_id = task_get_current_task_id();

  uart_puts(ANSI_GREEN"[scheduler]"ANSI_RESET" Task id: ");
  string_longlong_to_char(id_char, (long)current_task_id);
  uart_puts(id_char);
  uart_puts(" has entered the wait queue\n");

  task_guard_section();
  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, 2);
  task_unguard_section();

  schedule_enqueue_wait(current_task_id);
  schedule_yield();
  return;
}

void task_end_waiting(void)
{
  char id_char[0x10];

  /* put the first task in the wait queue back to running queue */
  uint64_t task_id = schedule_dequeue_wait();

  /* Some task in wait queue might be zombie */
  while(CHECK_BIT(kernel_task_pool[TASK_ID_TO_IDX(task_id)].flag, TASK_STATE_ZOMBIE))
  {
    task_id = schedule_dequeue_wait();
  }

  uart_puts(ANSI_GREEN"[scheduler]"ANSI_RESET" Task id: ");
  string_longlong_to_char(id_char, (long)task_id);
  uart_puts(id_char);
  uart_puts(" has left the wait queue\n");

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

