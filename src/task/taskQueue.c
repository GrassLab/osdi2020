#include "type.h"
#include "task/taskStruct.h"

struct task_node task_node_pool[64];
uint32_t task_node_count = -1;
struct task_queue tq = {
    .head = 0,
    .tail = 0,
    .q_size = 0};

struct task_node *_getTaskNode()
{
    task_node_count = (task_node_count + 1) % 64;
    return &task_node_pool[task_node_count];
}

void pushQueue(struct task *ready_task)
{
    struct task_node *empty_node = _getTaskNode();
    empty_node->cur_task = ready_task;
    empty_node->next = 0;

    if (tq.head == 0)
    {
        tq.head = empty_node;
    }
    if (tq.tail == 0)
    {
        tq.tail = empty_node;
    }
    else
    {
        tq.tail->next = empty_node;
        tq.tail = empty_node;
    }

    tq.q_size++;
}

struct task *popQueue()
{
    if (tq.q_size > 0)
    {
        struct task *next = tq.head->cur_task;
        tq.head = tq.head->next;
        tq.q_size--;

        if (tq.q_size == 0)
            tq.tail = 0;

        return next;
    }
    else
        return -1;
}