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

void *popQueue(Queue *self) {
    if (self->front == self->back) {
        sendStringUART("[ERROR] Queue is empty.\n");
        return NULL;
    }

    void *retval = self->buffer[self->front];
    self->front = (self->front + 1) % self->capacity;
    return retval;
}
