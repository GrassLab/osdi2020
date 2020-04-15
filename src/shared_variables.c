#include "shared_variables.h"

void shared_variables_init() {
    arm_core_timer_jiffies = 0;
    arm_local_timer_jiffies = 0;

    queue_init(&read_buf, UART0_BUF_MAX_SIZE);
    queue_init(&write_buf, UART0_BUF_MAX_SIZE);
}