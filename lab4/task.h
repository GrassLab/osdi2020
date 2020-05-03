#ifndef __TASK_H__
#define __TASK_H__

#define TASK_POOL_SIZE 64
#define TASK_KERNEL_STACK_SIZE 0x400
#define TASK_USER_STACK_SIZE 0x600

struct cpu_context_struct
{
  /* callee-saved registers */
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t fp; /* x29 */
  uint64_t lr; /* x30 */
  uint64_t sp;
  uint64_t spsr_el1;
  uint64_t elr_el1;
  uint64_t sp_el0;
};

struct task_struct
{
  uint64_t id;
  uint64_t quantum_count;
  /* bit */
  /* bit 0 -> reschedule */
  uint64_t flag;
  struct cpu_context_struct cpu_context;
};


void task_privilege_task_create(void(*start_func)());
uint64_t task_get_current_task_id(void);
void task_idle(void);
void task_privilege_demo(void);
void task_do_exec(void(*start_func)());
void task_user_demo(void);
void task_user_context1_demo(void);
void task_user_context2_demo(void);

#endif

