#include "schedule.h"
#include "string.h"
#include "task.h"
#include "timer.h"
#include "uart.h"
unsigned int runtaskcount;
static task_t init_task = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						   0, /* state etc */
						   0,
						   0,
						   0};

task_t *task_pool[64] = {
	&(init_task),
};
user_task_context user_pool[64];

void init()
{
	while (1)
	{
		schedule();
	}
}

static int task_count = 0;
static char kstack_pool[64][THREAD_SIZE] = {0};
static char ks[64][THREAD_SIZE] = {0};
static char ustack_pool[64][THREAD_SIZE] = {0};

int privilege_task_create(unsigned long func, int usr, int isexec)
{
	int task_id;
	task_id = task_count;
	task_count++;
	runtaskcount++;

	task_t *p = (task_t *)&kstack_pool[task_id][0];
	task_pool[task_id] = p;

	if (!p)
	{
		printf("errrrr");
		return -1;
	}
	p->state = TASK_RUNNING;
	p->counter = 2;

	if (usr)
	{
		p->is_usr = 1;
		user_pool[task_id].elr_el1 = func;
		user_pool[task_id].sp = (unsigned long)&ustack_pool[task_id][THREAD_SIZE];
		printf(" sp = > %x\n", user_pool[task_id].sp);
		if (isexec == 1)
		{
			printf("nnnew\n");
			p->cpu_context.x19 = (unsigned long)&jmp_to_new_usr;
		}
		else
		{
			p->cpu_context.x19 = (unsigned long)&jmp_to_usr;
		}
	}
	else
	{
		p->cpu_context.x19 = func;
	}

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)&ks[task_id][THREAD_SIZE];

	return task_id;
}

void task_init()
{
	int task_id = privilege_task_create((unsigned long)init, 0, 0);
	task_pool[task_id]->state = TASK_IDLE;
	runtaskcount--;
	set_current(task_id);
}

void context_switch(int task_id)
{
	int prev_id = get_current();
	set_current(task_id);
	task_t *now = task_pool[task_id];
	if (now->is_usr)
	{
		now->cpu_context.pc = now->cpu_context.x19;
	}
	switch_to(task_pool[prev_id], task_pool[task_id]);
}

static int schedule_index = 0;

void schedule()
{
	int task_id;
	task_t *p;
	while (1)
	{
		task_id = (++schedule_index) % task_count;
		p = task_pool[task_id];
		if (p && p->state == TASK_RUNNING)
		{
			break;
		}
	}
	printf("====switch  to %d ====\n", task_id);
	context_switch(task_id);
}

void timer_tick()
{
	task_t *current = task_pool[get_current()];
	// printf("counter = %d\n", current->counter);
	--current->counter;
	if (current->counter > 0)
	{
		// printf("nono\n");
		return;
	}
	printf("switch\n");
	current->counter = 2;
	current->switchflag = 1;
	if (current->is_usr)
	{
		jmp_to_usr();
	}
}

void jmp_to_usr()
{
	int now = get_current();
	task_t *current = task_pool[now];
	if (current->switchflag == 1)
	{
		printf("user context switch\n");
		current->switchflag = 0;
		schedule();
	}
	printf("jmp_to_usr = fun = %x  stack = %x\n", user_pool[now].elr_el1,
		   user_pool[now].sp);
	asm volatile("msr sp_el0, %0" ::"r"(user_pool[now].sp)
				 :);
	asm volatile("msr spsr_el1, %0" ::"r"(0)
				 :);
	asm volatile("msr elr_el1, %0" ::"r"(user_pool[now].elr_el1)
				 :);

	ret_to_usr();
}

void jmp_to_new_usr()
{
	int now;
	printf("jmp_new_task\n");
	now = get_current();
	asm volatile("msr sp_el0, %0" ::"r"(user_pool[now].sp)
				 :);
	asm volatile("msr spsr_el1, %0" ::"r"(0)
				 :);
	asm volatile("msr elr_el1, %0" ::"r"(user_pool[now].elr_el1)
				 :);
	asm volatile("eret");
}

void do_exec(unsigned long fun)
{
	int task_id = get_current();
	for (int i = 0; i < THREAD_SIZE; i++)
	{
		ustack_pool[task_id][THREAD_SIZE] = 0;
	}
	user_pool[task_id].elr_el1 = fun;
	user_pool[task_id].sp = (unsigned long)&ustack_pool[task_id][THREAD_SIZE];
	printf("exec success , goto %x\n", fun);
	asm volatile("msr sp_el0, %0" ::"r"(user_pool[task_id].sp)
				 :);
	asm volatile("msr spsr_el1, %0" ::"r"(0)
				 :);
	asm volatile("msr elr_el1, %0" ::"r"(user_pool[task_id].elr_el1)
				 :);
	asm volatile("eret");
};

void exec(unsigned long rfun)
{
	unsigned long rf = rfun;
	printf("exec = %x\n", rf);
	asm volatile("mov x1, %0" ::"r"(rf)
				 :);
	asm volatile("mov x0, #2");
	asm volatile("svc #0");
}
int get_taskid()
{
	asm volatile("mov x0, #9");
	asm volatile("svc #0");
	unsigned long current;
	asm volatile("mov %[result], x0"
				 : [ result ] "=r"(current));
	printf("ret = %x \n", current);
	return current;
}
int do_get_current() { return get_current(); }

int fork()
{
	asm volatile("mov x0, #7");
	asm volatile("svc #0");
	int rev;
	unsigned long stack;
	asm volatile("mov %[result], x0"
				 : [ result ] "=r"(rev));
	asm volatile("mov %[result], sp"
				 : [ result ] "=r"(stack));
	printf("ret stack = %x \n", stack);
	return rev;
}

int do_fork()
{
	unsigned long fun;
	asm volatile("MRS %[result], elr_el1"
				 : [ result ] "=r"(fun));
	unsigned long stack;
	asm volatile("MRS %[result], sp_el0"
				 : [ result ] "=r"(stack));
	int task_id = task_count;
	printf("fork new child => fun = %x user_stack = %x retsp = %x\n", fun,
		   &ustack_pool[task_id][THREAD_SIZE], stack);
	int nowid = get_current();

	memcpy((unsigned short *)ks[task_id], (unsigned short *)ks[nowid],
		   THREAD_SIZE);
	// printf("copy user stack   %x <= %x\n", &ustack_pool[task_id][THREAD_SIZE],
	//        &ustack_pool[nowid][THREAD_SIZE]);

	for (int i = 0; i < THREAD_SIZE; i++)
	{
		ustack_pool[task_id][i] = ustack_pool[nowid][i];
	}

	task_t *p = (task_t *)&kstack_pool[task_id][0];
	task_pool[task_id] = p;

	if (!p)
	{
		printf("error");
		return -1;
	}

	p->state = TASK_RUNNING;
	p->counter = 2;
	p->is_usr = 1;
	user_pool[task_id].elr_el1 = fun;
	user_pool[task_id].sp = (&ustack_pool[task_id][THREAD_SIZE]) -
							((&ustack_pool[nowid][THREAD_SIZE]) - stack);
	p->cpu_context.x19 = (unsigned long)&jmp_to_usr;
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	unsigned long getsp = 0;
	asm volatile("mov %[result], sp"
				 : [ result ] "=r"(getsp));
	p->cpu_context.sp =
		(&ks[task_id][THREAD_SIZE]) - (&ks[nowid][THREAD_SIZE] - getsp);
	printf("ksp : %x -> %x %x\n", getsp, p->cpu_context.sp,
		   &ks[nowid][THREAD_SIZE]);
	task_count++;
	runtaskcount++;
	return task_id;
}

void do_exit()
{
	task_t *now = task_pool[get_current()];
	now->state = TASK_IDLE;
	runtaskcount--;
	schedule();
}
void exit(int i)
{
	asm volatile("mov x0, #8");
	asm volatile("svc #0");
}

void foo1()
{
	printf("foo");
	while (1)
	{
		printf("Task id: %d\n", get_current());
		wait_cycles(10000000);
		int now = get_current();
		task_t *current = task_pool[now];
		if (current->switchflag == 1)
		{
			printf("user context switch\n");
			current->switchflag = 0;
			schedule();
		}
		// schedule();
	}
}

void idle1()
{
	while (1)
	{
		schedule();
		wait_cycles(10000000);
	}
}

void test1()
{
	task_init();
	for (int i = 0; i < 4; ++i)
	{ // N should > 2
		privilege_task_create((unsigned long)&foo1, 0, 0);
	}
	privilege_task_create((unsigned long)&idle1, 0, 0);
	core_timer_enable();
	schedule();
}

void tt()
{
	while (1)
	{
		printf("tt\n");
		wait_cycles(10000000);
	}
}

void ut1()
{
	while (1)
	{
		int cnt = 1;
		printf("test1 %x \n", &cnt);
		wait_cycles(10000000);
	}
}
void ut2()
{
	while (1)
	{
		int cnt = 1;
		printf("test2 %x \n", &cnt);
		wait_cycles(10000000);
	}
}
void user_test1() { do_exec((unsigned long)&ut1); }
void user_test2() { do_exec((unsigned long)&ut2); }

void idle()
{
	while (1)
	{
		printf("idle\n");
		schedule();
		wait_cycles(10000000);
	}
}

void test2()
{
	task_init();
	privilege_task_create((unsigned long)&idle, 0, 0);
	privilege_task_create((unsigned long)&user_test1, 0, 0);
	privilege_task_create((unsigned long)&user_test2, 0, 0);
	core_timer_enable();
	schedule();
}

void foo2()
{
	printf("i hate exec\n");
	exit(0);
}

void test()
{

	int cnt = 1;
	printf("run tests");
	int pid = fork();
	if (pid == 0)
	{
		fork();
		fork();
		printf("child\n");
		while (cnt < 10)
		{
			printf("cTask id: %d, cnt: %d\n", get_taskid(), cnt);
			wait_cycles(100000);
			++cnt;
		}
		exit(0);
		printf("Should not be printed\n");
	}
	else
	{
		printf("parent\n");
		printf("cnt = %d , %x\n", cnt, &cnt);
		while (cnt < 10)
		{
			printf("fTask id: %d, cnt: %d\n", get_taskid(), cnt);
			wait_cycles(100000);
			++cnt;
		}
		exec((unsigned long)&foo2);
		exit(0);
	}
}

void user_test() { do_exec((unsigned long)&test); }

void test3()
{
	task_init();
	privilege_task_create((unsigned long)&idle, 0, 0);

	privilege_task_create((unsigned long)&user_test, 0, 0);
	core_timer_enable();
	printf("runtaskcount = %d\n", runtaskcount);
	schedule();
}
