#ifndef TASK_H
#define TASK_H

typedef struct task {
    int id;
} Task;

typedef struct taskManager {
    int count;
    Task tasks[64];
} TaskManager;

void privilege_task_create(void(*func)());
void context_switch(Task* next);
void schedule();
#endif