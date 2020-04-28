#include "mem.h"
#include "task.h"

pcb_t * allocate_pcb ( uint64_t tid )
{
    pcb_t * pcb = ( pcb_t * ) ( ( KERNEL_MEMORY_LOW ) +  ( PCB_SIZE * tid ) );
    
    pcb -> kernel_stack_ptr = (uint64_t *)(((char *)pcb) + PCB_SIZE);
    pcb -> thread_info = ( thread_info_t * ) ( pcb );

    return pcb;
}