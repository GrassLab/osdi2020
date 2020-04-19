#ifndef _QUEUE_H
#define _QUEUE_H

#include "uart.h"
int isempty(int head,int tail);
int isfull(int head,int tail);

char pop(char* q,int* head);
void push(char* q,int* tail,char c);

int runQ_pop(int* q,int* head);
void runQ_push(int* q,int* tail,int c);

#endif /*_QUEUE_H*/
