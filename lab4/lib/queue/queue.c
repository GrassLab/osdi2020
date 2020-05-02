#include "queue/queue.h"
#include "MiniUart.h"

void pushQueue(Queue *self, void *task) {
    size_t next_back = (self->back + 1) % self->capacity;
    if (next_back == self->front) {
        sendStringUART("[ERROR] Queue is full.\n");
        return;
    }

    self->buffer[next_back] = task;
    self->back = next_back;
}

bool isQueueEmpty(Queue *self) {
    return (self->front == self->back) ? : false;
}

void *popQueue(Queue *self) {
    if (isQueueEmpty(self)) {
        sendStringUART("[ERROR] Queue is empty.\n");
        return NULL;
    }

    self->front = (self->front + 1) % self->capacity;
    void *retval = self->buffer[self->front];
    return retval;
}
