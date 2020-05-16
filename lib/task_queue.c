#include "task_queue.h"
#include "schedule.h"

void task_queue_init(struct task_queue_t* q) {
    q->front = 0;
    q->rear = 0;
}

void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task) {
    elmt->task = task;
    elmt->next = 0;
    elmt->prev = 0;
}

void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt) {
    if (q->front == 0) {
        q->front = elmt;
        q->rear = elmt;
    }
    // task priority is largest
    else if (elmt->task->priority > q->front->task->priority) {
        q->front->next = elmt;
        elmt->prev = q->front;
        q->front = elmt;
    }
    // task priority is smallest
    else if (elmt->task->priority <= q->rear->task->priority) {
        q->rear->prev = elmt;
        elmt->next = q->rear;
        q->rear = elmt;
    }
    // q->front->priority >= elmt->task->priority > q->last->priority
    else {
        // find appropriate place to insert
        struct task_queue_elmt_t *ptr = q->rear;
        while (ptr->next != 0 && elmt->task->priority > ptr->task->priority) {
            ptr = ptr->next;
        }
        // push elmt to back of ptr
        elmt->next = ptr;
        elmt->prev = ptr->prev;
        // relink before and after element
        ptr->prev->next = elmt;
        ptr->prev = elmt;
    }
}

struct task_t* task_queue_pop(struct task_queue_t* q) {
    if (q->front == 0) {
        return &task_pool[0];
    }
    else if (q->front == q->rear) {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        pop_elmt->next = 0;
        pop_elmt->prev = 0;
        q->front = 0;
        q->rear = 0;
        return pop_task;
    }
    else {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        q->front = pop_elmt->prev;
        pop_elmt->next = 0;
        pop_elmt->prev = 0;
        return pop_task;
    }
}