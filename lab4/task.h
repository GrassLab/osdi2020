#ifndef __TASK_H__
#define __TASK_H__

typedef struct context_def{
	unsigned long long x19;
	unsigned long long x20;
	unsigned long long x21;
	unsigned long long x22;
	unsigned long long x23;
	unsigned long long x24;
	unsigned long long x25;
	unsigned long long x26;
	unsigned long long x27;
	unsigned long long x28;
	unsigned long long fp;
	unsigned long long *lr;
	unsigned long long *sp;
}context_t;

typedef struct task_def{
	int taskId;
	context_t context;
}task_t;

void test();
void privilege_task_create( void(*func)() );


/* ===================== task.S ===================== */
void switch_to(context_t *prev, context_t *next);
int get_cur_taskId(void);
void set_cur_taskId(int taskId);



#endif