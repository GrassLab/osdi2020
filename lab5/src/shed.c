#include "tools.h"
#include "mmu.h"
#include "mm.h"
#include "exception.h"
#include "sys.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
struct task_struct * u_task[NR_TASKS] = {&(init_task), };

int n_tasks = 1;
int n_task_id = 1;


void enable_preempt(){
    current->preempt_count++;
}
void disable_preempt(){
    current->preempt_count--;
}

int privilege_task_create(void (*func)()){
    disable_preempt();
    struct task_struct *new_task = (struct task_struct *) get_free_page();
    memset((unsigned short *)new_task, 0, PAGE_SIZE);

    new_task->taskid = n_task_id;
    new_task->counter = 1;
    new_task->state = TASK_RUNNING;
    new_task->cpu_context.x19 = (unsigned long)func;
    new_task->cpu_context.pc = (unsigned long)ret_from_fork;
    new_task->cpu_context.sp = (unsigned long)new_task + THREAD_SIZE;
    new_task->parentid = current->taskid;
    n_task_id++;
    

    
    new_task->storeid = n_tasks;
    struct task_struct *new_utask = (struct task_struct *) get_free_page();
    u_task[n_tasks] = new_utask;
    
    task[n_tasks] = (new_task);

    n_tasks++; 
    n_tasks%=NR_TASKS;

       
    // if(n_tasks>=NR_TASKS)
    //     for(int i=1;i<NR_TASKS;i++){
    //         if(task[i]->state==TASK_ZOMBIE){
    //             task[i] = (new_task);                
    //             break;
    //         }
    //     }
    // else{
    //     
    // }
    uart_puts("Create new Task: ");
    uart_send_int(new_task->taskid);
    uart_send('\n');
    enable_preempt();
    return new_task->taskid;
}
int fork(){
    
    int id = privilege_task_create(0);
    
    struct task_struct *fork_task = task[id];
    fork_task->counter = 1;
    fork_task->cpu_context.x19 = current->cpu_context.x19;
	fork_task->cpu_context.x20 = current->cpu_context.x20;
	fork_task->cpu_context.x21 = current->cpu_context.x21;
	fork_task->cpu_context.x22 = current->cpu_context.x22;
	fork_task->cpu_context.x23 = current->cpu_context.x23;
	fork_task->cpu_context.x24 = current->cpu_context.x24;
	fork_task->cpu_context.x25 = current->cpu_context.x25;
	fork_task->cpu_context.x26 = current->cpu_context.x26;
	fork_task->cpu_context.x27 = current->cpu_context.x27;
	fork_task->cpu_context.x28 = current->cpu_context.x28;
	fork_task->cpu_context.fp = current->cpu_context.fp;
    fork_task->cpu_context.pc = current->cpu_context.pc;
    fork_task->taskid = n_task_id;
    fork_task->parentid = current->taskid;
    fork_task->storeid = n_tasks;

    struct task_struct *new_utask = u_task[id];
    struct task_struct *old_utask = u_task[current->taskid];
    new_utask->cpu_context.x19 = old_utask->cpu_context.x19;
    new_utask->cpu_context.pc = old_utask->cpu_context.pc;
    new_utask->cpu_context.sp = old_utask->cpu_context.sp;

    u_task[n_tasks] = new_utask;
    task[n_tasks] = (fork_task);


    n_task_id++;
    n_tasks++; 
    n_tasks%=NR_TASKS;

    return 0;
}
void exec(void (*func)()){
    asm volatile("msr sp_el0, %0"::"r"(u_task[current->taskid]):);
	asm volatile("msr spsr_el1, %0"::"r"(0):);
	asm volatile("msr elr_el1, %0"::"r"(func):);
	asm volatile("eret");
}

void foo(){
    int a=5;
  while(1) {
    uart_puts("Task id: ");
    uart_send_int(current -> taskid);
    uart_send('\n');
    uart_puts("address: a: ");
    uart_hex(&a);
    uart_send('\n');
    uart_puts("Remaining page frames : ");
    uart_send_int(remain_page_num());
    uart_send('\n');
    exit(0);
  }
}
void test_command3() { // test page reclaim.
  uart_puts("Remaining page frames : ");
  uart_send_int(remain_page_num());
  uart_send('\n');
}
void foo2(){
  int* a=0x0;
  while(1) {
    uart_puts("a: ");
    uart_send_int(a);
    uart_send('\n');

    delay(100000000);
  }
}
void foofoo(){
  int cnt = 0;
  if(fork() == 0) {
    while(cnt < 10) {
      //printf("task id: %d, sp: 0x%llx cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      uart_puts("task id: ");
      uart_send_int(call_sys_get_taskid());
      uart_puts("\n");
      uart_puts("address: ");
      uart_hex(&cnt);
      uart_puts("\n");
      uart_puts("cnt: ");
      uart_send_int(cnt++);
      uart_puts("\n");
      delay(1000000);
    }
    exit(0); // all childs exit
  }
}
void foo_lab4(){
    int tmp = 5;
    uart_puts("Task id: ");
    uart_send_int(call_sys_get_taskid());
    uart_puts(" after exec, tmp address ");
    uart_hex((unsigned int)(&tmp));
    uart_puts(", tmp value ");
    uart_send_int(tmp);
    uart_puts("\n");
    call_sys_exit();
}

// 1000000
void test() {
    int cnt = 1;
    delay(100000000);
    if (fork() == 0) {
        // fork();
        delay(100000000);
        // fork();
        while(cnt < 5) {
            call_sys_write("Task id: ");
            uart_send_int(call_sys_get_taskid());
            call_sys_write(", Parent task id: ");
            uart_send_int(current->parentid);
            call_sys_write(", cnt: ");
            uart_send_int(cnt);
            call_sys_write("\n");
            delay(100000000);
            ++cnt;
        }
        call_sys_exit();
        uart_puts("Should not be printed\n");
    } else {
        uart_puts("Task id: ");
        uart_send_int(call_sys_get_taskid());
        uart_puts(" after exec, cnt address ");
        uart_hex((unsigned int)(&cnt));
        uart_puts(", cnt value ");
        uart_send_int(cnt);
        uart_puts("\n");
        foo_exec(foo_lab4);
        call_sys_exit();
    }
}
void foo_exec(){
    call_exec(foo2);
}
void foo_sys(){
    fork();
    uart_puts("Task id: ");
    uart_send_int(call_sys_get_taskid());
    uart_send('\n');
    delay(100000000);
    call_sys_exit();
}

void idle(){
  while(1){
    schedule();
    delay(100000000);
  }
}
int get_taskid(){
    return current->taskid;
}
void exit(){
    disable_preempt();
    // for (int i = 0; i < NR_TASKS; i++){
    //     if (task[i] == current) {
    //         task[i]->state = TASK_ZOMBIE;
    //         break;
    //     }
    // }
    int id = current->storeid;
    current->state = TASK_ZOMBIE;
    
    free_page(pa_to_pfn((unsigned long)task[id]));
    free_page(pa_to_pfn((unsigned long)u_task[id]));
    enable_preempt();
    schedule();
}
void init_init_task(void (*func)()){
    struct task_struct *p;
    p = (struct task_struct *) get_free_page();

    memset((unsigned short *)p, 0, PAGE_SIZE);

    p->taskid = 0;
    p->state = TASK_RUNNING;
    p->counter = 0;
    p->cpu_context.x19 = (unsigned long)func;
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)current + THREAD_SIZE;
    p->parentid = -1;
    p->storeid = 0;
    task[0] = p;

    current = p;

    struct task_struct *new_utask = (struct task_struct *) get_free_page();
    new_utask->cpu_context.x19 = (unsigned long)func;
    new_utask->cpu_context.pc = (unsigned long)ret_from_fork;
    new_utask->cpu_context.sp = (unsigned long)current + THREAD_SIZE;
    u_task[0] = new_utask;
}

void switch_to(struct task_struct *next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next; 
	cpu_switch_to(prev, next);
    
}

void schedule(){
    disable_preempt();
    int next, c;
    // while(1){
    c = 0;
    next = 0;
    for(int i=1;i<NR_TASKS;i++){
        if(task[i]&&task[i]->counter>c&&task[i]->state==TASK_RUNNING){
            c = task[i]->counter;
            next=i;
            break;
        }
    }
        // if(c)break;

    // }
    // int next;
    // next = ((current->taskid)+1)%n_tasks;
    switch_to(task[next]);
    enable_preempt();
}

int N = 5;
void create_foo(){
    for(int i = 1; i <= N; ++i) { // N should > 2
        privilege_task_create(foo);
    }
    // schedule();
}

void update_task_counter(){
    if(current->preempt_count<0)return;
    current->counter--;
    // uart_puts("current counter: \n");
    // uart_send_int(current->counter);
    // uart_send('\n');

    if(current->counter>0)
        return;
    current->counter=0;
    enable_irq();
    schedule();
    // disable_irq();
}