#include "include/printf.h"
#include "include/queue.h"
#include "include/scheduler.h"

int isempty(int head,int tail){
	return head==tail;
}

int isfull(int head,int tail){
	return (tail+1)%BUF_SIZE == head;
}

char pop(char* q,int* head){ //head call by reference since we want to change its value
	char tmp;
	tmp = q[*head];
	*head = ((*head)+1)%BUF_SIZE;
	return tmp;
}

void push(char* q,int* tail,char c){ //tail call by reference since we want to change its value
	q[*tail] = c;
	*tail = ((*tail)+1)%BUF_SIZE;
}

/************ function for priority queue ***********/

void init_priority_queue(){
	runqueue.capacity = QUEUE_SIZE;
	runqueue.heap_size = 0;
	
	waitqueue.capacity = QUEUE_SIZE;
	waitqueue.heap_size = 0;
}

// A utility function to swap two elements and keys
void swap(struct node *x, struct node *y)
{
    struct node temp = *x; 
    *x = *y; 
    *y = temp; 
}
// to get index of parent of node at index i 
int parent(int i) { return (i-1)/2; }
// to get index of left child of node at index i 
int left(int i) { return (2*i + 1); }
// to get index of right child of node at index i
int right(int i) { return (2*i + 2); }

void priorityQ_push(struct priority_queue *q,int key, int element){
	if(q->heap_size >= q->capacity){
		//somthing wrong...
		printf("Can't insert more in heap\r\n");
		while(1);
		return; 
	}
	
	int i = q->heap_size;
	
	q->node[i].key = key;
	q->node[i].element = element;

	while(i !=0 && q->node[parent(i)].key < q->node[i].key){
		swap(&q->node[i], &q->node[parent(i)]);
		i = parent(i);
	}
	q->heap_size++;
}

int priorityQ_pop(struct priority_queue *q){
	if(q->heap_size<=0){
		return -1; 
	}
	else{
		q->heap_size--;

		if(q->heap_size == 0)
			return q->node[0].element;
		else{
			struct node root = q->node[0];
			q->node[0].key = q->node[q->heap_size].key;
			q->node[0].element = q->node[q->heap_size].element;

			heapify(q,0);
			
			return root.element;
		}
		
	}
}

void heapify(struct priority_queue *q,int i){
    int l = left(i); 
    int r = right(i); 
    int largest = i;

    if (l < q->heap_size && q->node[l].key >= q->node[i].key) 
        largest = l; 
    if (r < q->heap_size && q->node[r].key >= q->node[largest].key) 
        largest = r; 
    
    if (largest != i) 
    { 
        swap(&q->node[i], &q->node[largest]); 
        heapify(q,largest); 
    } 
}

