#ifndef _QUEUE_H
#define _QUEUE_H

#include "uart.h"

#define QUEUE_SIZE 64 // NR_TASKS

struct node{
	int key;
	int element; 
};

// using a heap as priority queue
struct priority_queue{
	struct node node[QUEUE_SIZE]; // key and element in heap
	int capacity; // maximum possible size of min heap 	
	int heap_size; // Current number of elements in min heap 
};

// A priority base runQ
struct priority_queue runqueue;

int isempty(int head,int tail);
int isfull(int head,int tail);
char pop(char* q,int* head);
void push(char* q,int* tail,char c);


/* function for prioirity queue */
void init_priority_queue();
void priorityQ_push(struct priority_queue *q,int key, int element);
int priorityQ_pop(struct priority_queue *q);
void heapify(struct priority_queue *q,int i);
#endif /*_QUEUE_H*/
