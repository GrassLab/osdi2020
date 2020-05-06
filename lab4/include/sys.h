extern int get_taskid();
extern void uart_read(char buf[]);
extern void uart_write(const char buf[]);
extern int fork();
extern void exec();
extern void exit(int status);
void set_trap_ret(struct task* task_base, long long ret_val, int reg_offset);
long get_trap_arg(int reg_offset);