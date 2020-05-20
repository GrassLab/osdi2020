
#include "util.h"
#include "test.h"
#include "shell.h"
#include "usrlib.h"
#include "string.h"
#include "sprintf.h"
#include "syscall.h"

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
  int child = 0, k = 0;
  println("parent user process");
  println("user_process_fork test" NEWLINE);
  child = call_sys_fork();
  if(child) k += 1;
  while(1){
    printf("===============> k = [0x%x] = %d" NEWLINE, &k, k);
    if(!child) println("===============> child user process");
    else println("===============> parent user process");
    delay(500000);
    k += 2;
  }
}

void test_demand_paging(){
  int *p = (int*) 0x4123;
  *p = 3;
  println(NEWLINE "pass the test, demand paging works");
}

int main(){
  //fork_test();
  shloop();
  //test_command3();
  //test_demand_paging();
  //read_beyond_boundary();
  //write_beyond_boundary();
  //wrong_permission();
  //test_stack_overflow();
  //mmaps();
  //mmap_unalign();
  //write_text();
  return 0;
}
