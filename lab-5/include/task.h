#ifndef TASK_H
#define TASK_H

#include "memory.h"

#define IN_KERNEL_MODE 0
#define IN_USER_MODE 1
#define ZOMBIE 2

#define MAX_PROCESS_PAGES 16	

typedef struct cpuContext {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long sp; // kstack sp
    unsigned long pc;
} __attribute__ ((aligned (8))) CpuContext;

typedef struct userContext {
    unsigned long sp_el0;   // user stack
    unsigned long elr_el1;  // user pc 
    unsigned long spsr_el1; // user cpu state
} __attribute__ ((aligned (8))) UserContext;

typedef struct trapframe {
	unsigned long regs[31];
} Trapframe;

typedef struct pageInfo {
    unsigned long pgd;
	int userPageCount;
	int kernelPageCount;
	Page userPages[MAX_PROCESS_PAGES];
	unsigned long kernelPages[MAX_PROCESS_PAGES];
} PageInfo;

typedef struct task {
    CpuContext cpuContext;
    UserContext userContext;
    PageInfo pageInfo;
    int id;
    int rescheduleFlag;
    int timeCount;
    int state; // IN_KERNEL_MODE || IN_USER_MODE || ZOMBIE
    int parentId;
    unsigned long trapframe;
} Task;

typedef struct taskManager {
    int taskCount;
    int runningTaskId;
    Task taskPool[64];
    char ustackPool[64][4096];
    char kstackPool[64][4096];
} TaskManager;


void task_manager_init(void(*func)());
void privilege_task_create(void(*func)());
void context_switch(Task* next);
void schedule();
Trapframe* get_task_trapframe(struct task *task);
void do_exec(unsigned long start, unsigned long size, void(*func)());
void __exit(int status);
int __fork();

// Lab4 test case
void req12_test();
void req34_test();
void uart_test();
void test();
void hello();
void idle12();
void idle();
void foo12();
void foo();
#endif