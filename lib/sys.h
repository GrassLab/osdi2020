#ifndef __LIB_SYS_H
#define __LIB_SYS_H

#include "type.h"

/* defined in sys.S */
extern uint32_t get_board_revision();
extern uint64_t get_vc_base_addr();
extern void svc_test();
extern void irq_enable();
extern void irq_disable();
extern void core_timer_enable();
extern void core_timer_disable();
extern void local_timer_enable();
extern void local_timer_disable();

#endif