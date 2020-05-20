#define SYSN_CORE_TIMEER_ENABLE 0
#define SYSN_SHOW_TIMESTAMP 1
#define SYSN_REBOOT 2
#define SYSN_SCHEDULE 3
#define SYSN_GET_TASK_ID 4 
#define SYSN_UART_READ 5
#define SYSN_UART_WRITE 6
#define SYSN_EXEC 7
#define SYSN_FORK 8
#define SYSN_EXIT 9
#define SYSN_UART_HEX 10
#define SYSN_UART_SEND 11
#define SYSM_PRINT 12
#define Idle_size 0x007ffff;

void core_time_enable();
void reboot();
void timestamp();
void show_hex(unsigned long long a);
void puts(char *c);
void send(char c);
void exec(void(*func)());
int fork();
void exit(int x);
char getc();
void schedule();
int get_task_id();
void printf(const char *fmt, ...);
void delay();
int syscall();
