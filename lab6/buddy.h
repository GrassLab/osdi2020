#ifndef __BUDDY_H__
#define __BUDDY_H__

#define PFN_NUM (262144)
#define BUDDY_MAX (18)
#define BUDDY_ALLOCATED_NUM (32)
#define BUDDY_NODE_LIST_NUM (16)

typedef struct{
	int start;
	int end;
	int allocated;
} buddy_node;

typedef struct{
	int num;
	buddy_node node_list[BUDDY_NODE_LIST_NUM];
} buddy_head;

typedef struct{
	int num;
	buddy_node node_list[BUDDY_ALLOCATED_NUM];
}buddy_allocated;



void buddy_init( void );
int buddy_alloc(int page_request_num);
void buddy_free(int node_start);
#endif