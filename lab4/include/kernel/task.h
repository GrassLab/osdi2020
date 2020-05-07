#ifndef _TASK_STRUCT_H_
#define _TASK_STRUCT_H_
#include "stdint.h"
typedef int32_t pid_t;

struct task_t {
	// struct saved_context context;
	uint64_t saved_reg[10]; // x19-x28
	uint64_t fp_lr[2]; //x29 x30
	uint64_t ksp;
	uint64_t sp_el0;
	uint64_t elr_el1;
	uint64_t spsr_el1;
	uint64_t trapframe;
	uint64_t kbase;
	uint64_t ubase;
	pid_t task_id;
	uint8_t state;
	uint8_t priority;
	uint8_t alive;
	uint8_t privilege;
};

extern volatile uint64_t bitmap;
extern struct task_t task_pool[64];

int privilege_task_create(void (*func)(void));
struct task_t *get_running_task();
// int32_t do_exec(void (*func)(), int is_privilege);
void exec();
int32_t do_fork();
void print_task();

#endif