#ifndef __UART_Q__
#define __UART_Q__

#define UART_QUEUE_MAX_SIZE 2048

struct uart_queue {  // circular queue
    int front;
    int rear;
    int max;
    char buf[UART_QUEUE_MAX_SIZE];
};

#endif

void uart_queue_init(struct uart_queue* q, int max);
int uart_queue_empty(struct uart_queue* q);
int uart_queue_full(struct uart_queue* q);
void uart_queue_push(struct uart_queue* q, char val);
char uart_queue_pop(struct uart_queue* q);

// ------------------- //

#ifndef __TASK_Q__
#define __TASK_Q__

struct task_queue_elmt_t {  /* priority queue */
    struct task_t* task;
    struct task_queue_elmt_t* prev;
    struct task_queue_elmt_t* next;
};

struct task_queue_t {
    struct task_queue_elmt_t* front;
    struct task_queue_elmt_t* rear;
};

#endif

void task_queue_init(struct task_queue_t* q);
void task_queue_elmt_init(struct task_queue_elmt_t* elmt, struct task_t *task);
void task_queue_push(struct task_queue_t* q, struct task_queue_elmt_t* elmt);
struct task_t* task_queue_pop(struct task_queue_t* q);
void task_queue_print(struct task_queue_t* q);
