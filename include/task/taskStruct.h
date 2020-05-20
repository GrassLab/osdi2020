#ifndef _TASKSTRUCT_H
#define _TASKSTRUCT_H

#include "type.h"

struct cpu_context
{
	uint64_t x19; //0
	uint64_t x20;
	uint64_t x21; //1
	uint64_t x22;
	uint64_t x23; //2
	uint64_t x24;
	uint64_t x25; //3
	uint64_t x26;
	uint64_t x27; //4
	uint64_t x28;
	uint64_t fp; //5
	uint64_t pc;
	uint64_t sp; //6
	uint64_t elr_el1;
	uint64_t sp_el0; //7
};

typedef enum
{
	ready,
	running,
	pending,
	zombie
} state;

#define MAX_PROCESS_PAGES			16	

struct user_page {
	unsigned long phys_addr;
	unsigned long virt_addr;
};

struct mm_struct {
	unsigned long pgd;
	int user_pages_count;
	struct user_page user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	unsigned long kernel_pages[MAX_PROCESS_PAGES];
};

struct task
{
	struct cpu_context kernel_context;
	uint32_t task_id;
	uint32_t priority;
	state task_state;
	bool re_schedule;
	struct mm_struct mm;
};

struct task_node
{
	struct task *cur_task;
	struct task_node *next;
};

struct task_queue
{
	struct task_node *head;
	struct task_node *tail;
	size_t q_size;
};

#endif