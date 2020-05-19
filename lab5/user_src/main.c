
#include "sys.h"
#include "util.h"
#include "shell.h"
#include "string.h"
#include "sprintf.h"

void exit() {
  while(1){
    call_sys_write("user_process_exit test" NEWLINE);
    call_sys_exit();
  }
}

#if 0
void user_login(){
  char buffer[128], pw[10] = "root";
  printf(NEWLINE "============      login daemon      ============"  NEWLINE);
  while(1){
    getline(buffer, '*');
    int d = strcmp(buffer, pw);
    if(!d) break;
    printf(NEWLINE "input password:");
  }
  println(NEWLINE "============     login successfully!     ============");
  exit();
}

void user_shell(){
  printf(NEWLINE "============      [%d] shell daemon      ============"  NEWLINE, current_task->pid);
  irq_shell_loop(0);
  exit();
}

void user_idle() {
  while(1){
    call_sys_write("user_process idle..." NEWLINE);
    delay(1000000);
  }
}

void user_hang() {
  printf(NEWLINE "============      user  hang       ============"  NEWLINE);
  while(1){
    delay(1000000);
    printf(NEWLINE "============     user is hanging    ============"  NEWLINE);
  }
}
void user_exec() {
  while(1){
    call_sys_write("user_process_exec test" NEWLINE);
    call_sys_exec(user_idle);
  }
}

void user_fork() {
  int child = 0, k = 15;
  call_sys_write("user_process_fork test" NEWLINE);
  child = call_sys_fork();

  while(1){
    //printf("stack reg = %x - %x" NEWLINE, ustack_pool[current_task->pid % TASK_SIZE],
    //    ustack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE);
    //printf("[%d] fork return %d" NEWLINE, current_task->pid, child);
    printf("k = [0x%x] = %d" NEWLINE, &k, k);
    call_sys_write(NEWLINE);
    delay(1000000);
    if(!child) k++;
  }
}

#endif

#if support_mutex
Mutex gmtx = {0, 0};

void user_mutex(){
  call_sys_mutex_lock(&gmtx);
  for(int i = 0; i < 5; i++){
    printf(NEWLINE "============      [%d] hold mtx [%d]      ============"  NEWLINE, current_task->pid, i);
    delay(1000000);
  }
  call_sys_mutex_unlock(&gmtx);
  exit();
  //while(1){
  //  call_sys_exit();
  //  delay(1000000);
  //}
}
#endif

void read_test() {
  call_sys_write("============         READ    test        ============" NEWLINE);
  while(1){
    printf("input:");
    printf("read %d" NEWLINE, call_sys_read());
  }
}

void write_test() {
  while(1){
    call_sys_write(NEWLINE
        "============         WRITE   test        ============"
        NEWLINE);
    delay(1000000);
  }
}

void user_test(){
  call_sys_write("user task checkpoint 1" NEWLINE);
  delay(1000000);
  call_sys_write("user task checkpoint 2" NEWLINE);
  delay(1000000);
  call_sys_write("user task checkpoint 3" NEWLINE);
}

void fork_test(){
  int child = 0, k = 15;
  println("parent user process");
  println("user_process_fork test" NEWLINE);
  //child = call_sys_fork();
  while(1){
    //printf("stack reg = %x - %x" NEWLINE, ustack_pool[current_task->pid % TASK_SIZE],
    //    ustack_pool[current_task->pid % TASK_SIZE] + STACK_SIZE);
    //printf("[%d] fork return %d" NEWLINE, current_task->pid, child);
    printf("===============> k = [0x%x] = %d" NEWLINE, &k, k);
    if(child) println("===============> child user process");
    else println("===============> parent user process");
    delay(10000);
    if(!child) k++;
  }
}

int main(){
  //fork_test();
  shloop();
  return 0;
}
