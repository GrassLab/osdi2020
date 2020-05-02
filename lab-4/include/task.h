#ifndef TASK_H
#define TASK_H

typedef struct context {
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
    unsigned long sp;
    unsigned long pc;
} Context;

typedef struct task {
    Context context;
    int id;
} Task;

typedef struct taskManager {
    int taskCount;
    Task taskPool[64];
    char kstackPool[64][4096];
} TaskManager;


void taskManagerInit();
void privilege_task_create(void(*func)());
void context_switch(Task* next);
void schedule();
void foo();
#endif