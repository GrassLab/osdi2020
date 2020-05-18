#include "lib.h"


void delay(unsigned long num)
{
    for (unsigned long i = 0; i < num; i++)
    {
        asm volatile("nop");
    }
}

void test_command1() { // test fork functionality
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      //printf("task id: %d, sp: 0x%llx cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      uart_puts("task_id: ");
      uart_send_hex(get_taskid());
      uart_puts("sp: ");
      uart_send_hex((unsigned int)(unsigned long)&cnt);
      uart_puts("cnt: ");
      uart_send_hex(cnt++);

      delay(1000000);
    }
    exit(0); // all childs exit
  }
}

void test_command2() { // test page fault
  if(fork() == 0) {
    int* a = 0xffff0000f0000000; // a non-mapped address.
    uart_send_hex(*a);
  }
}

void test_command3() { // test page reclaim.
    uart_puts("Remaining page frames :0x");
    uart_send_hex(remain_page_num());
    uart_send('\n');
    uart_send('\r');
  //printf("Remaining page frames : %d\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}

void print()
{
    char str[] = "\n=====\nuser program test\n=====\n";
    char *ptr = str;
    while (*ptr != '\0')
    {
        uart_send(*ptr);
        ptr++;
    }
}

#define BUFFER_SIZE 64
int main()
{
    int x = 0;

    uart_puts("Shell start\n");
    // main loop

    print();

    char cmd[BUFFER_SIZE];
    while (x != -1)
    {
        uart_puts(">");

        // get command
        memset(cmd, 0, BUFFER_SIZE);
        uart_gets(cmd, BUFFER_SIZE);

        uart_send('\r');
        uart_send('\n');

        if (strcmp(cmd, "") == 0){

        }
        else if(strcmp(cmd, "reboot") == 0){
            reboot();
        }
        else if(strcmp(cmd, "exit") == 0){
            exit(0);
        }
        else if(strcmp(cmd, "hello") == 0){
            uart_puts("Hello, World\n");
        }
        else if(strcmp(cmd, "t1") == 0){
            test_command1();
        }
        else if(strcmp(cmd, "t2") == 0){
            test_command2();
        }
        else if(strcmp(cmd, "t3") == 0){
            test_command3();
        }
        else if(strcmp(cmd, "bad") == 0){
            *((unsigned long *) 0xffff0000f0000000) = 0;
        }
        else if(strcmp(cmd, "") == 0){

        }
        else{
            uart_send('<');
            uart_puts(cmd);
            uart_puts(">: Command not find\n\r");
        }

        // default command
        // x = sh_default_command(cmd);
    }

}
