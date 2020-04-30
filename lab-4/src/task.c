#include "task.h"
#include "uart.h"

int taskCount = 0;
Task tasks[64];
extern Task* get_current();
extern void switch_to(struct task* prev, struct task* next);
char kstack_pool[64][4096];

void example_task_one()
{
    int count = 10000;
    while(count-- > 0);
    uart_puts("1....\n");
    context_switch(&tasks[1]);
}

void example_task_two()
{
    int count = 10000;
    while(count-- > 0);
    uart_puts("2....\n");
    context_switch(&tasks[0]);
}

void privilege_task_create(void(*func)())
{

}

void context_switch(Task* next)
{
    Task *prev = get_current();
    switch_to(prev, next);
}

void schedule() 
{

}