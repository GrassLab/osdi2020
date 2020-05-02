#define QUEUE_EMPTY(queue)  queue.head == queue.tail
#define QUEUE_FULL(queue, size) queue.head == (queue.tail + 1) % size
#define ENQUEUE(queue, size, c)  queue.tail = (queue.tail + 1) % size; queue.buf[queue.tail] = c
#define DEQUEUE(queue, size, retv) queue.head = (queue.head + 1) % size; retv = queue.buf[queue.head]