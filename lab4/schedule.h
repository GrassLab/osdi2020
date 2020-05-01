void schedule();
void context_switch(int task_id);
void task_init();
int privilege_task_create(unsigned long func);
void scs();
void ics();
void timer_tick();