#define SYS_timer_enable 0
#define SYS_time 1
#define SYS_exec 2
#define SYS_printf 3
#define SYS_fork 7
#define SYS_exit 8
#define SYS_getpid 9

.global _lib_start_main
_lib_start_main:
    bl usr_main

.global myprintf
myprintf:
    mov x5,x4
    mov x4,x3
    mov x3,x2
    mov x2,x1
    mov x1,x0
    mov x0,SYS_printf
	svc #0
	ret