/* #include <stdint.h> */

/* #include "io.h" */
/* #include "lfb.h" */
/* #include "mbox.h" */
/* #include "reset.h" */
/* #include "shell.h" */
/* #include "syscall.h" */
/* #include "task.h" */
/* #include "uart.h" */

/* void loop_user() { */
/* sys_uart_write("echo_user \n", 11); */
/* print_s("loop user\n"); */
/* char c; */
/* while (1) { */
/* c = read_c(); */
/* print_c(c); */
/* } */
/* } */

/* void exec_user() { */
/* print_s("exec user (call loop user)\n"); */
/* sys_exec(loop_user); */
/* while (1) */
/* ; */
/* } */

/* void fork_exit_user() { */
/* print_s("fork exit user\n"); */
/* int a = sys_fork(); */
/* if (a == 0) { */
/* print_s("pid: "); */
/* print_i(a); */
/* print_s("\n"); */
/* while (1) */
/* ; */
/* } else { */
/* print_s("pid: "); */
/* print_i(a); */
/* print_s("\n"); */
/* sys_fork(); */
/* sys_exit(2); */
/* print_s("Should not be printed\n"); */
/* } */
/* } */

/* void kill_user() { */
/* print_s("kill user\n"); */
/* sys_kill(2, SIGKILL); */
/* while (1) */
/* ; */
/* } */

/* void echo_user() { */
/* sys_uart_write("echo_user \n", 11); */
/* char ch[10]; */
/* sys_uart_read(ch, 1); */
/* print_s("your input: "); */
/* sys_uart_write(ch, 1); */
/* print_s("\n"); */
/* while (1) */
/* ; */
/* } */

/* void kexit_task() { */
/* print_s("task 1\n"); */
/* kexit(3); */
/* } */

/* void exec_task() { */
/* print_s("exec task\n"); */
/* do_exec(exec_user); */
/* } */

/* void echo_task() { */
/* print_s("echo task\n"); */
/* do_exec(echo_user); */
/* } */

/* void fork_exit_task() { */
/* print_s("fork exit task\n"); */
/* do_exec(fork_exit_user); */
/* } */

/* void kill_task() { */
/* print_s("kill task (kill process 2)\n"); */
/* do_exec(kill_user); */
/* } */

/* void loop_task() { */
/* print_s("loop kernel task\n"); */
/* do_exec(loop_user); */
/* } */

/* void loop_ktask() { */
/* print_s("loop kernel task\n"); */
/* while (1) */
/* ; */
/* } */

/* void sche_task() { */
/* print_s("schedule task\n"); */
/* while (1) */
/* ; */
/* } */
