

typedef struct task_t{
    int id;
    void (*func)();
}task_t;

task_t task_pool[64];

static int task_pool_len = 0;

static char kstack_pool[64][4096];

static int runqueue[64];

static int runqueue_len = 0;

void privilege_task_create(void(*func)()){

    task_t task = {
        task_pool_len,
        func
    };

    task_pool[task_pool_len++] = task;

    char *stack = kstack_pool[task.id];

    // asm volatile("mov sp, %0" : "=r"(stack));

    runqueue[runqueue_len++] = task.id;

}

extern void switch_to(task_t*, task_t*);

extern struct task_t* get_current();

void context_switch(task_t *next){
    next->func();
    // switch_to(get_current(), next);
}

static int schedule_index = 0;

int schedule_cnt = 0;

void schedule(){
    // uart_send_int(schedule_cnt);
    
    schedule_index = (schedule_index + 1) % task_pool_len;
    int task_id = schedule_index;
    task_pool[task_id].func();


}

int check_reschedule(){
    // every 500ms, reschdule
    if(schedule_cnt > 500){
        schedule_cnt = 0;
        return 1;
    }
    else
        return 0;
}
