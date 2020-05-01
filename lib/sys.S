.section ".text"

#include "kernel/exception/sys_call_def.h"

.global get_board_revision
.global get_vc_base_addr
.global svc_test
.global irq_enable
.global irq_disable
.global core_timer_enable
.global core_timer_disable
.global local_timer_enable
.global local_timer_disable

get_board_revision:
    mov     x8,     SYS_GET_VC_BASE_ADDR
    svc     #0
    ret    

get_vc_base_addr:
    mov     x8,     SYS_GET_BOARD_REVISION
    svc     #0
    ret

svc_test:
    mov     x8,     SYS_TEST_SVC
    svc     #0
    ret

irq_enable: 
    mov     x8,     SYS_IRQ_EL1_ENABLE
    svc     #0
    ret

irq_disable :
    mov     x8,     SYS_IRQ_EL1_DISABLE
    svc     #0
    ret

core_timer_enable:
    mov     x8,     SYS_CORE_TIMER_ENABLE 
    svc     #0
    ret

core_timer_disable:
    mov     x8,     SYS_CORE_TIMER_DISABLE
    svc     #0
    ret

local_timer_enable:
    mov     x8,     SYS_LOCAL_TIMER_ENABLE
    svc     #0
    ret

local_timer_disable:
    mov     x8,     SYS_LOCAL_TIMER_DISABLE
    svc     #0
    ret