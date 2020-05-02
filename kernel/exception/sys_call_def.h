#ifndef __SYS_SYS_CALL_DEF
#define __SYS_SYS_CALL_DEF

#define SYS_TEST_SVC            1
#define SYS_IRQ_EL1_ENABLE      3
#define SYS_IRQ_EL1_DISABLE     4
#define SYS_CORE_TIMER_ENABLE   5
#define SYS_CORE_TIMER_DISABLE  6
#define SYS_GET_TIMESTAMP       7
#define SYS_SCHEDULE            8
#define SYS_DO_EXEC             9
#define SYS_WAIT_MSEC           10
#define SYS_DO_EXIT             11
#define SYS_GET_PID             12
#define SYS_UART_WRITE          13
#define SYS_UART_READ           14
#define SYS_GET_VC_BASE_ADDR    15
#define SYS_GET_BOARD_REVISION  16
#define SYS_LOCAL_TIMER_ENABLE  17
#define SYS_LOCAL_TIMER_DISABLE 18
#define SYS_FORK                19

#endif