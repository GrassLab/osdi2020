unsigned long call_sys_get_taskid();
unsigned long exec(void(*func)());
int fork();
unsigned long exit();
char call_sys_uart_read();
void call_sys_uart_write(char *s);
void call_sys_printf(char *s);