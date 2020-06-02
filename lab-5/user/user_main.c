#include "userlib.h"

void test_command1() { // test fork functionality
    int cnt = 0;
    if(fork() == 0) {
        fork();
        fork();
        while(cnt < 10) {
        printf("task id: %d, sp: 0x%llx cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
        delay(1000000);
        }
        exit(0); // all childs exit
    }
}

void test_command2() { // test page fault
    if(fork() == 0) {
        int* a = 0x0; // a non-mapped address.
        printf("%d\n", *a); // trigger simple page fault, child will die here.
    }
}

void test_command3() { // test page reclaim.
  printf("Remaining page frames : %d\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}

int main()
{

    return 0;
}