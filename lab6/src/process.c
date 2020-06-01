#include "io.h"
#include "task.h"
#include "util.h"
#include "sched.h"
#include "allocator.h"

void zombie_reaper(){
  printf(NEWLINE "============      [%d] zombie reaper     ============"
      NEWLINE, current_task->pid);
  while(1){
    printf(NEWLINE "============     [%d] zreaper running    ============"
        NEWLINE, current_task->pid);
    for(int i = 0; i < TASK_SIZE; i++){
      if(tasks[i] && tasks[i]->status == zombie){
        printf(NEWLINE "============ ZombieReaper kill zombie %d ============"
            NEWLINE, tasks[i]->pid);
        tasks[i]->status = none;
        tasks[i] = 0;
      }
    }
    delay(500000);
    schedule();
  }
}

#define TASK_(n) void task_ ## n () { \
  printf(NEWLINE "============      [%d] TASK" #n " daemon      ============" \
      NEWLINE, current_task->pid); \
  while(1){ \
    printf(NEWLINE "============      [%d] TASK" #n " running     ============" \
        NEWLINE, current_task->pid); \
    delay(5000000); \
    check_resched(); \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

extern unsigned long _binary____usrbuild_user_img_start;
extern unsigned long _binary____usrbuild_user_img_end;
extern void user_entry();

void user_hang() {
  printf(NEWLINE "============      user  hang       ============"  NEWLINE);
  while(1){
    delay(1000000);
    printf(NEWLINE "============     user is hanging    ============"  NEWLINE);
  }
}

void kexec_user_main(){
  printf(NEWLINE "============     [%d] kexec user main     ============"
      NEWLINE, current_task->pid);
  unsigned long begin = (unsigned long)&_binary____usrbuild_user_img_start;
  unsigned long end = (unsigned long)&_binary____usrbuild_user_img_end;
  unsigned long process = (unsigned long)begin - (unsigned long)begin;//(unsigned long)&user_entry;
  int err = move_to_user_mode(begin, end - begin, process);
  if (err < 0){
    printf("Error while moving process to user mode\n\r");
  }
}

void test_val(unsigned long addr){
  for(int i = -10; i < -10; i++){
    printfmt("%x the val 0x%x", addr + 48 + i * 8, *(unsigned long*)(addr + 48 + i * 8));
  }
}

void task_fixed_aloc() { 

  printf(NEWLINE "============    [%d] TASK  allocation    ============"
      NEWLINE, current_task->pid);


  for(unsigned size = 1; size < 515; size++){

    unsigned times = 10;
    unsigned long addrs[times];
    unsigned long token = fixed_get_token(size);

    for(int i = 0; i < times; i++){
      addrs[i] = fixed_alloc(token);
      printfmt("addr = 0x%x", addrs[i]);
    }
    for(int i = 0; i < times; i++){
      fixed_free(token, addrs[i]);
    }

    fixed_free_token(token);
    printfmt("size = %d", size);

  }
  printf(NEWLINE "============    [%d] TASK  alloc done    ============"
      NEWLINE, current_task->pid);
  exit();
}

void task_varied_aloc() { 

  printf(NEWLINE "============    [%d] TASK  allocation    ============"
      NEWLINE, current_task->pid);

  unsigned long token = varied_get_token();
  for(unsigned size = 1; size < 515; size++){

    unsigned times = 10;
    unsigned long addrs[times];

    for(int i = 0; i < times; i++){
      addrs[i] = varied_alloc(token, size);
      printfmt("addr = 0x%x", addrs[i]);
    }
    for(int i = 0; i < times; i++){
      varied_free(token, addrs[i]);
    }
    printfmt("size = %d", size);
  }
  varied_free_token(token);
  printf(NEWLINE "============    [%d] TASK  alloc done    ============"
      NEWLINE, current_task->pid);
  exit();
}

void kernel_process(){
  puts("kernel process begin...");
  //printf("kstack: %x, ustack %x" NEWLINE, kstack_pool, ustack_pool);
  //privilege_task_create(do_exec, (UL)user_login, 3);
  //privilege_task_create(do_exec, (UL)user_shell, 2);
  privilege_task_create(task_1, 0, current_task->priority);
  //privilege_task_create(task_3, 0, current_task->priority);
  //privilege_task_create(task_fixed_aloc, 0, current_task->priority);
  privilege_task_create(task_varied_aloc, 0, current_task->priority);
  //privilege_task_create(task_3, 0, current_task->priority);
  //privilege_task_create(task_4, 0, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_fork, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_exec, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_mutex, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_write, current_task->priority);
  //privilege_task_create(do_exec, (UL)user_hang, current_task->priority);
  //privilege_task_create(kexec_user_main, 0, current_task->priority);
  privilege_task_create(zombie_reaper, 0, current_task->priority);
  //privilege_task_create(task_2, 0);
  //privilege_task_create(task_3, 0);
  //privilege_task_create(task_4, 0);
  //irq_shell_loop();
  //puts("kernel stack:");
  //show_sp();
  /* do_exec((unsigned long)user_exit); */
  //exit();
  //do_exec((unsigned long)user_write);
  exit();
  while(1){
    puts("kernel process scheduling");
    schedule();
  }
}
