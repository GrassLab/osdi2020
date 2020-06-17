#define TASK_IDLE 0
#define TASK_RUNNING 1
#define THREAD_SIZE 4096

void schedule();
void context_switch(int task_id);
void task_init();
int privilege_task_create(unsigned long func, int usr);
void timer_tick();
void test1();
void test2();
void test3();
void exec(unsigned long fun);
void do_exec(unsigned long fun);
void jmp_to_usr();
void jmp_to_new_usr();
int do_fork();
void exit(int i);
void do_exit();
int get_taskid();
int do_get_current();
typedef struct cpu_context_t
{
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long sp;
    unsigned long pc;
} cpu_context_t;

#define MAX_PROCESS_PAGES 16

struct user_page
{
    unsigned long phys_addr;
    unsigned long virt_addr;
};

struct mm_struct
{
    unsigned long pgd;
    int user_pages_count;
    struct user_page user_pages[MAX_PROCESS_PAGES];
    int kernel_pages_count;
    unsigned long kernel_pages[MAX_PROCESS_PAGES];
};

typedef struct task_t
{
    cpu_context_t cpu_context;
    long state;
    long counter;
    int is_usr;
    int switchflag;
    struct mm_struct mm;
} task_t;

typedef struct user_task_context
{
    unsigned long elr_el1;
    unsigned long sp;
    char page_table[1000];
} user_task_context;

task_t *task_pool[64];