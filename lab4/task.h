#ifndef __TASK_H__
#define __TASK_H__

/* ===================== define ===================== */
typedef struct context_def{
	unsigned long long must_preserve_reg[10];
	unsigned long long fp;
	unsigned long long lr;
	unsigned long long kstack;
}context_t;

typedef enum{
	run,
	ready,
	wait,
	zombie
}processStatus;

typedef struct task_def{
	context_t context;

	unsigned long long ustack;
	unsigned long long umode_lr;

	int taskId;
	int ustack_id;
	int exitStatus;
	processStatus pstatus;

}task_t;

#define MAX_QUEUE 16
typedef struct queue_def{
	int circular_queue[MAX_QUEUE];
	int front;
	int rear;
	int isFull;
}queue_t;


/* ===================== outer file use ===================== */
extern int ReSchedule;
extern queue_t runQueue;
extern task_t *idle_pcb;


int privilege_task_create( void(*func)() );
void schedule( void );
void init_Queue(queue_t *q);
void idle( void );
void do_exec( void(*func)() );
void kernel_routine_entry( void );
void kernel_routine_exit( void );
void do_fork( void );
void do_exit(int status);
void zombieReaper( void );

/* ===================== task.S ===================== */
void store_umode_lr_sp(unsigned long long *elr_el1, unsigned long long *ustack);
void restore_umode_lr_sp(unsigned long long elr_el1, unsigned long long ustack);
void switch_to(context_t *prev, context_t *next);
task_t* get_cur_task( void );
void set_cur_task(task_t *task);
void go_to(context_t *idle_context);
void restore_all_reg( void );



#endif