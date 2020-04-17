#include "include/uart.h"

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

