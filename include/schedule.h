#include "typedef.h"

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "vfs.h"

struct task_queue_elmt_t {  /* priority queue */
    struct task_t* task;
    struct task_queue_elmt_t* prev;
    struct task_queue_elmt_t* next;
};

struct task_queue_t {
    struct task_queue_elmt_t* front;
    struct task_queue_elmt_t* rear;
};

#define TASK_EPOCH 1
#define TASK_POOL_SIZE 64
#define KSTACK_SIZE 4096
#define KSTACK_TOP_IDX (KSTACK_SIZE - 16) // sp need 16bytes alignment
#define USTACK_ADDR (0x0000ffffffffe000 - 8)

struct cpu_context {
    // ARM calling convention
    // x0 - x18 can be overwritten by the called function
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
    uint64_t fp;  // x29
    uint64_t lr;  // x30
    uint64_t sp;
};

#define MAX_USER_PAGES      16
#define MAX_KERNEL_PAGES    16

struct mm_struct {
    uint64_t pgd;
};

#define current_task        get_current_task()

enum task_state {
    RUNNING,
    ZOMBIE,
    EXIT,
};

struct task_t {
    uint64_t id;
    enum task_state state;
    int priority;
    int counter;
    int need_resched;
    int exit_status;
    struct cpu_context cpu_context;
    struct mm_struct mm;
    struct files_struct files;
    struct dentry* pwd;
};

/* Variables init in schedule.c */
extern struct task_t *task_pool;
extern char kstack_pool[TASK_POOL_SIZE][KSTACK_SIZE];

/* Function in schedule.S */
struct task_t* get_current_task();
void update_current_task(struct task_t *task);
void update_pgd(uint64_t pgd);
void switch_to(struct cpu_context* prev, struct cpu_context* next);

/* Function in schedule.c */
void task_queue_init(struct task_queue_t* q);
void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task);
void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt);
struct task_t* task_queue_pop(struct task_queue_t* q);
void task_queue_print(struct task_queue_t* q);

void task_init();
void schedule_init();
int privilege_task_create(void (*func)(), int priority);
void context_switch(struct task_t* next);
void schedule();
int do_exec(uint64_t start, uint64_t size, uint64_t pc);
void do_exit(int status);

#endif
