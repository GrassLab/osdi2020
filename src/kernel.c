#include "exce.h"
#include "mailbox.h"
#include "mm.h"
#include "printf.h"
#include "schedule.h"
#include "svc.h"
#include "timer.h"
#include "uart.h"

void delay(int cnt)
{
    while(cnt--);
}

void wait(long task_id){
    #ifdef __DEBUG
    printf("[wait] Task_id: %d\n", current->task_id);
    #endif//__DEBUG
    while (1)
    {
        if(task_pool[task_id]->state == TASK_ZOMBIE){
            task_pool[task_id] = 0;
            break;
        }
        sys_sched_yield();
    }
    
}

void foo(){// user code
    int tmp = 5;
    printf("Task %d after  exec, tmp address 0x%x, tmp value %d\n", sys_get_taskid(), &tmp, tmp);
    sys_exit(0);
}

void test() {//user code
    int cnt = 1;
    int pid;
    #ifdef __DEBUG
    printf("[test] Task_id: %d, cnt: %d\n", current->task_id, cnt);
    #endif//__DEBUG
    if ((pid = sys_fork()) == 0) {
        sys_fork();
        delay(100000);
        sys_fork();
        #ifdef __DEBUG
        printf("[tast] Task_id: %d, cnt: %d\n", current->task_id, cnt);
        #endif//__DEBUG
        while(cnt < 10){
            printf("Task id: %d, cnt: %d\n", sys_get_taskid(), cnt);
            delay(100000);
            ++cnt;
        }
        sys_exit(0);
    printf("Should not be printed\n");
    } else {
        printf("Task %d before exec, cnt address 0x%x, cnt value %d\n", sys_get_taskid(), &cnt, cnt);
        wait(pid);
        sys_exec(foo);
  }
}

void user_test(){//kernel code
    do_exec(test);
}

int kernel_main()
{
    uart_init();
    init_printf(0, uart_putc);  
    get_board_info();

    // create test process
    // privilege_task_create(user_test);
    // for(int i = 0; i < 5; ++i) { // N should > 2
    //     privilege_task_create(foo);
    // }

    mm_init();
    //buddy test
    struct page* addr = __buddy_block_alloc(1);
    struct page* addr2 = __buddy_block_alloc(5);
    struct page* addr3 = __buddy_block_alloc(5);
    __buddy_block_free(addr3);
    __buddy_block_free(addr2);
    __buddy_block_free(addr);

    //object allocator.full test
    __init_obj_page(&bookkeep[1], 64);
    int token0 = register_obj_allocator(64);
    int token1 = register_obj_allocator(32);
    void *obj0[128];// should allocate two page
    void *obj1[256];// should allocate two page
    for(int i=0; i<128; i++){
        obj0[i] = obj_allocate(token0);
        obj1[2*i  ] = obj_allocate(token1);
        obj1[2*i+1] = obj_allocate(token1);
    }
    // object allocator.partial test
    obj_free(obj0[25]);
    obj_free(obj0[26]);
    obj0[25] = obj_allocate(token0);
    obj0[26] = obj_allocate(token0);
    // object allocator.empty test
    for(int i=0; i<128; i++){
        obj_free(obj0[i]);
    }
    obj0[0] = obj_allocate(token0);

    core_timer_enable();
    idle_task();
    return -1;
}
