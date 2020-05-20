#include "util.h"
#include "usrlib.h"
#include "sprintf.h"

void test_command1() { // test fork functionality
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      delay(1000000);
    }
    exit(0); // all childs exit
  }
}

void test_command2() { // test page fault
  if(fork() == 0) {
    int* a = (int*)(4 << 20); // a non-mapped address.
    printf("%d\n", *a); // trigger simple page fault, child will die here.
  }
}

void test_command3() { // test page reclaim.
  printf("Remaining page frames : %d\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}

void read_beyond_boundary(){
  if(fork() == 0) {
    int* ptr = mmap(NULL, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("addr: 0x%x\n", ptr);
    printf("%d\n", ptr[1000]); // should be 0
    printf("%d\n", ptr[4097]); // should be seg fault
  }
}

void write_beyond_boundary(){
  if(fork() == 0) {
    int* ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    printf("addr: %llx\n", ptr);
    ptr[1000] = 100;
    printf("%d\n", ptr[1000]); // should be 100
    ptr[4097] = 100;// should be seg fault
    printf("%d\n", ptr[4097]); // not reached
  }
}

void wrong_permission(){
  if(fork() == 0) {
    int* ptr = mmap(NULL, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("addr: %x\n", ptr);
    printf("%d\n", ptr[1000]); // should be 0
    for(int i = 0; i < 4096; ++i) {
      printf("i = %d %x" NEWLINE, i, ptr + i);
      ptr[i] = i+1; // should be seg fault
    }
    for(int i = 0; i < 4096; ++i) { // not reached
      printf("%d\n", ptr[i]);
    }
  }
}

#pragma GCC optimize("O0")
int stack_overflow(int i){
  int a[1024] = {0};
  printf("Recursive %d\n", i);
  stack_overflow(i + 1);
  return a[1023] + i;
}

void test_stack_overflow(){
  if(fork() == 0) {
    stack_overflow(0);
  }
}


void mmaps(){ // test multiple mmaps
  if(fork() == 0) {
    for(int i = 0; i < 40; ++i){
      if ( i < 20 ) {
        mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, -1, 0);
      } else if(i < 30){
        mmap(NULL, 4096, PROT_WRITE, MAP_ANONYMOUS, -1, 0);
      } else {
        mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, -1, 0);
      }
    }
    printf("[done] task id is [%d]" NEWLINE, call_sys_task_id());
    while(1); // hang to let shell see the mapped regions
  }
}

void mmap_unalign(){
  if(fork() == 0) {
    // 0x12345678 0x1fff exceed limit
    printf("0x%x", mmap((void*)0x4999, 0x1fff, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, -1, 0)); // should be a page aligned address A and region should be A - A +0x2000
    while(1); // hang to let shell see the mapped regions
    printf("[done] task id is [%d]" NEWLINE, call_sys_task_id());
  }
}

void write_text() {
  if(fork() == 0){
    int* pc;
    __asm__ volatile(
      "adr %0, ."
      :"=r"(pc)
    );
    *pc = 0; // seg fault
  }
}
