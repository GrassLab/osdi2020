#ifndef __TASK_H__
#define __TASK_H__

void task_privilege_task_create(void(*start_func)());
uint64_t task_get_current_privilege_task_id(void);
void task_idle(void);
void task_demo(void);

#endif

