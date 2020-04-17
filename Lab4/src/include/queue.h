#ifndef _QUEUE_H
#define _QUEUE_H

#include "uart.h"
int isempty(int head,int tail);
int isfull(int head,int tail);

char pop(char* q,int* head);
void push(char* q,int* tail,char c);
#endif /*_QUEUE_H*/
