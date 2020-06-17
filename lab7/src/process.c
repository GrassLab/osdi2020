#include "io.h"
#include "task.h"
#include "util.h"
#include "sched.h"
#include "shell.h"
#include "string.h"
#include "allocator.h"
#include "type.h"
#include "fs.h"

void zombie_reaper(int show){
  char msg[20];
  show_task_msg("zombie reaper");
  while(1){
    if(show) show_task_msg("zreaper running");
    for(int i = 0; i < TASK_SIZE; i++){
      if(tasks[i] && tasks[i]->status == zombie){
        sprintf(msg, "reaper kill zombie %d", tasks[i]->pid);
        show_task_msg(msg);
        tasks[i]->status = none;
        tasks[i] = 0;
      }
    }
    delay(500000);
    schedule();
  }
}

#define TASK_(n) void task_ ## n () { \
  show_task_msg("TASK" #n " daemon"); \
  while(1){ \
    show_task_msg("TASK" #n " running"); \
    delay(5000000); \
    check_resched(); \
  } \
}

TASK_(1); TASK_(2); TASK_(3); TASK_(4);

extern unsigned long _binary____usrbuild_user_img_start;
extern unsigned long _binary____usrbuild_user_img_end;
extern void user_entry();

void user_hang() {
  show_task_msg("user hang");
  while(1){
    delay(1000000);
    show_task_msg("user is hanging");
  }
}

void kexec_user_main(){
  show_task_msg("kexec user main");
  unsigned long begin = (unsigned long)&_binary____usrbuild_user_img_start;
  unsigned long end = (unsigned long)&_binary____usrbuild_user_img_end;
  unsigned long process = (unsigned long)begin - (unsigned long)begin;
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

void task_buddy_aloc(int ret) {

  show_task_msg("TASK allocation");

  zone_show(buddy_zone, 5);

  const unsigned size = 6;
  const unsigned times = 3;
  unsigned long pages[size][times];

  for(unsigned s = 0; s < size; s++)
    for(int i = 0; i < times; i++)
      pages[s][i] = VA_START | zone_get_free_pages(buddy_zone, s);

  for(unsigned s = 0; s < size; s++)
    for(int i = 0; i < times; i++)
      zone_free_pages(buddy_zone, pages[s][i]);

  zone_show(buddy_zone, 5);
  show_task_msg("TASK alloc done");

  if(!ret) exit();
}

void task_fixed_aloc(int ret) {

  show_task_msg("TASK allocation");
  for(unsigned size = 1; size < 515; size += 127){

    unsigned times = 10;
    unsigned long addrs[times];
    unsigned long token = fixed_get_token(size);

    for(int i = 0; i < times; i++){
      addrs[i] = fixed_alloc(token);
    }
    for(int i = 0; i < times; i++){
      fixed_free(token, addrs[i]);
    }

    fixed_free_token(token);
  }
  show_task_msg("TASK alloc done");

  if(!ret) exit();
}

void task_varied_aloc(int ret) {

  show_task_msg("TASK allocation");

  unsigned long token = varied_get_token();
  for(unsigned size = 1; size < 66; size += 6){

    unsigned times = 1;
    unsigned long addrs[times];

    for(int i = 0; i < times; i++){
      addrs[i] = varied_alloc(token, size);
    }
    for(int i = 0; i < times; i++){
      varied_free(token, addrs[i]);
    }
  }
  varied_free_token(token);
  show_task_msg("TASK alloc done");

  if(!ret) exit();
}

void task_vnode_op(int ret){
  struct file* a = vfs_open("non_exist", 0);
  assert(a == NULL);
  a = vfs_open("hello", O_CREAT);
  assert(a != NULL);
  vfs_close(a);
  struct file* b = vfs_open("hello", 0);
  assert(b != NULL);
  vfs_close(b);
  if(!ret) exit();
}

void task_file_op(int ret){
  char buf[128];
  struct file* a = vfs_open("hello", O_CREAT);
  struct file* b = vfs_open("world", O_CREAT);
  vfs_write(a, "Hello ", 6);
  vfs_write(b, "World!", 6);
  vfs_close(a);
  vfs_close(b);
  b = vfs_open("hello", 0);
  a = vfs_open("world", 0);
  int sz = vfs_read(b, buf, 100);
  sz += vfs_read(a, buf + sz, 100);
  buf[sz] = '\0';
  puts(buf); // should be Hello World!
  if(!ret) exit();
}

void task_read_dir(int ret){
  // create some regular files at root directory
  struct file* root = vfs_open("read_dir_test_file", O_CREAT);
  // your read directory function
  DIR *dir = vfs_opendir("/");
  if(dir) list_dir(dir, 0);
  // iterate all directory entries and print each file's name.
  vfs_closedir(dir);
  vfs_close(root);
  if(!ret) exit();
}

void kernel_process(){
  puts("kernel process begin...");
  //privilege_task_create(task_1, 0, current_task->priority);
  //privilege_task_create(task_2, current_task->priority);
  //privilege_task_create(task_3, current_task->priority);
  //privilege_task_create(task_4, current_task->priority);
  //privilege_task_create(task_buddy_aloc, 0, current_task->priority);
  //privilege_task_create(task_fixed_aloc, 0, current_task->priority);
  //privilege_task_create(task_varied_aloc, 0, current_task->priority);
  privilege_task_create(zombie_reaper, 0, current_task->priority);
  //privilege_task_create(task_vnode_op, 0, current_task->priority);
  //privilege_task_create(task_file_op, 0, current_task->priority);
  //privilege_task_create(task_read_dir, 0, current_task->priority);
  privilege_task_create(kexec_user_main, 0, current_task->priority);
  privilege_task_create(irq_shell_loop, EL, current_task->priority);
  while(1){ delay(500000); schedule(); }
}
