#include "stdint.h"
#include "schedule.h"

struct task_queue_elmt_t {  /* priority queue */
    struct task_t* task;
    struct task_queue_elmt_t* prev;
    struct task_queue_elmt_t* next;
};

struct task_queue_t {
    struct task_queue_elmt_t* front;
    struct task_queue_elmt_t* rear;
};

void task_queue_init(struct task_queue_t* q);
void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task);
void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt);
struct task_t* task_queue_pop(struct task_queue_t* q);
void task_queue_print(struct task_queue_t* q);