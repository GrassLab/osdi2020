#include "exception/timer.h"
#include "schedule/task.h"
#include "schedule/context.h"
#include "MiniUart.h"


void _enable_core_timer(void) {
    // enable timer
    asm volatile("mov x0, 1");
    asm volatile("msr cntp_ctl_el0, x0");

    // set expired time for triggering first interrupt faster
    _core_timer_handler();

    // enable timer interrupt
    asm volatile("mov x0, 2");
    asm volatile("ldr x1, =0x40000040");
    asm volatile("str x0, [x1]");
}

void _core_timer_handler(void) {
    // expired after this # of ticks, this # is close to 1 sec
    asm volatile("mov x0, 0x1f00000");
    asm volatile("msr cntp_tval_el0, x0");

    sendStringUART("Hi, I'm timer\n");

    TaskStruct *cur_task = getCurrentTask(); 

    --cur_task->counter;
    if (cur_task->counter > 0) {
        return;
    }

    // time's up, release the resource
    cur_task->reschedule_flag = true;
}
