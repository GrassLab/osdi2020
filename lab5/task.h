#include "mmu.h"
#ifndef __TASK_H__
#define __TASK_H__

#define TASK_POOL_SIZE 64
#define TASK_KERNEL_STACK_SIZE 0x1000
#define TASK_USER_STACK_SIZE 0x1000

#define TASK_STATE_RESCHEDULE 0
#define TASK_STATE_ZOMBIE 1
#define TASK_STATE_WAIT 2
#define TASK_STATE_GUARD 3
#define TASK_STATE_USER_SPACE 4

#define TASK_PRIORITY_LOW 0u
#define TASK_PRIORITY_HIGH 1u

#define TASK_ID_TO_IDX(x) ((x - 1))

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
  uint64_t id; /* if id != idx + 1 then the slot is unused */
  uint64_t quantum_count;
  /* bit */
  /* bit 0 -> reschedule */
  /* bit 1 -> zombie */
  /* bit 2 -> wait state */
  /* bit 3 -> critical section */
  /* bit 4 -> has user space */
  uint64_t flag;
  uint64_t signal;
  uint64_t priority;
  struct user_space_mm_struct user_space_mm;
  struct cpu_context_struct cpu_context;
};


uint64_t task_privilege_task_create(void(*start_func)(), unsigned priority);
uint64_t task_get_current_task_id(void);
uint64_t task_user_get_current_task_id(void);
void task_test1(void);
void task_test2(void);
void task_test3(void);
void task_do_exec(uint64_t * start, uint64_t size);
uint64_t task_get_current_task_signal(void);
void task_start_waiting(void);
void task_end_waiting(void);
void task_guard_section(void);
void task_unguard_section(void);
int task_is_guarded(void);

extern char _binary_test1_bin_start;
extern char _binary_test1_bin_end;
extern char _binary_test2_bin_start;
extern char _binary_test2_bin_end;
extern char _binary_test3_bin_start;
extern char _binary_test3_bin_end;

#endif

