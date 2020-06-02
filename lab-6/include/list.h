#ifndef LIST_H
#define LIST_H


typedef struct list {
    struct list *head;
    struct list *tail;
} List;

#endif