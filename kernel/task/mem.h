#ifndef __SYS_MEM_H
#define __SUS_MEM_H

#include "kernel/peripherals/gpio.h"
#include "lib/type.h"

#include "task.h"

#define KERNEL_STACK_SIZE   (1 << 13)  /* 8K bytes */
#define USER_STACK_SIZE     (1 << 22)  /* 4M bytes */

#define NUM_THREADS         64

#define TASK_INFO_SIZE      (int)(sizeof(task_t))
// this is not working, and I don't know why
// #define PCB_SIZE          (KERNEL_STACK_SIZE + TASK_INFO_SIZE)
#define PCB_SIZE            16384
#define KERNEL_SPACE_SIZE   (NUM_THREADS * PCB_SIZE)

#define KERNEL_MEMORY_HIGH  ((volatile char*) MMIO_BASE )
#define KERNEL_MEMORY_LOW   ((volatile char*)((uint64_t)KERNEL_MEMORY_HIGH - KERNEL_SPACE_SIZE))
#define USER_MEMORY_HGIH    KERNEL_MEMORY_LOW
#define USER_MEMORY_LOW     ((volatile char*) 0x90000)  /* variable define in link.ld */

typedef struct {  

    thread_info_t * thread_info;
    uint64_t * kernel_stack_ptr;
    uint64_t * user_stack_ptr;
    
} pcb_t;

pcb_t * allocate_pcb ( uint64_t tid );

#endif