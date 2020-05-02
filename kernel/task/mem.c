#include "mem.h"

#include "lib/math.h"

#include "task.h"

/* 64 bit to maintain which one is in used */
uint64_t KERNEL_SPACE_USAGE = 0;
uint64_t USER_SPACE_USAGE   = 0;

pcb_t * allocate_pcb ( )
{
    /* find usable space */
    int kernel_space_index = find_first_0_in_bit ( KERNEL_SPACE_USAGE );
    KERNEL_SPACE_USAGE |= ( 0b1 << kernel_space_index );

    int user_space_index = find_first_0_in_bit ( USER_SPACE_USAGE );
    USER_SPACE_USAGE |= ( 0b1 << user_space_index );

    /* allocate memory */
    pcb_t * pcb = (pcb_t *) ( ( KERNEL_MEMORY_LOW ) + ( PCB_SIZE * kernel_space_index ) );

    pcb->kernel_stack_ptr = (uint64_t *) ( ( (char *) pcb ) + PCB_SIZE );

    /* don't care about text, data segment of user_space right now. */
    pcb->user_stack_ptr = (uint64_t *) ( ( USER_MEMORY_LOW ) + ( USER_STACK_SIZE * ( user_space_index + 1 ) ) );

    pcb->kernel_space_index = kernel_space_index;
    pcb->user_space_index   = user_space_index;

    pcb->thread_info = (thread_info_t *) ( pcb );

    pcb->thread_info->task_id = kernel_space_index;

    return pcb;
}

void release_pcb ( pcb_t * pcb )
{
    pcb->kernel_stack_ptr = NULL;
    pcb->user_stack_ptr   = NULL;

    release_kernel_space ( pcb->kernel_space_index );
    release_user_space ( pcb->user_space_index );
}

void release_kernel_space ( int index )
{
    if ( index != 0 )
        KERNEL_SPACE_USAGE = mask_0_in_bit ( KERNEL_SPACE_USAGE, index );
}

void release_user_space ( int index )
{
    if ( index != 0 )
        USER_SPACE_USAGE = mask_0_in_bit ( USER_SPACE_USAGE, index );
}
