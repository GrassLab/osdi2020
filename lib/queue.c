#include "schedule.h"
#include "uart0.h"
#include "queue.h"

/* UART Queue */

void uart_queue_init(struct uart_queue* q, int max) {
    q->front = 0;
    q->rear = 0;
    q->max = max;
}

int uart_queue_empty(struct uart_queue* q) {
    return q->front == q->rear;
}

int uart_queue_full(struct uart_queue* q) {
    return q->front == (q->rear + 1) % q->max;
}

void uart_queue_push(struct uart_queue* q, char val) {
    if (uart_queue_full(q)) return;  // drop if full
    q->buf[q->rear] = val;
    q->rear = (q->rear + 1) % q->max;
}

char uart_queue_pop(struct uart_queue* q) {
    if (uart_queue_empty(q)) return '\0';
    char elmt = q->buf[q->front];
    q->front = (q->front + 1) % q->max;
    return elmt;
}

/* Task Queue */

void task_queue_init(struct task_queue_t* q) {
    q->front = NULL;
    q->rear = NULL;
}

void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task) {
    elmt->task = task;
    elmt->next = NULL;
    elmt->prev = NULL;
}

void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt) {
    if (q->front == NULL) {
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
        while (ptr->next != NULL && elmt->task->priority > ptr->task->priority) {
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
    if (q->front == NULL) {
        return &task_pool[0];
    }
    else if (q->front == q->rear) {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        pop_elmt->next = NULL;
        pop_elmt->prev = NULL;
        q->front = NULL;
        q->rear = NULL;
        return pop_task;
    }
    else {
        struct task_queue_elmt_t* pop_elmt = q->front;
        struct task_t* pop_task = pop_elmt->task;
        q->front = pop_elmt->prev;
        q->front->next = NULL;
        pop_elmt->next = NULL;
        pop_elmt->prev = NULL;
        return pop_task;
    }
}

void task_queue_print(struct task_queue_t* q) {
    struct task_queue_elmt_t* ptr = q->front;
    while (ptr != q->rear->prev) {
        uart_printf("%d ", ptr->task->id);
        ptr = ptr->prev;
    }
    uart_printf("\n");
}
