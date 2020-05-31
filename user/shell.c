#include "my_string.h"
#include "uart0.h"
#include "util.h"
#include "exception.h"
#include "sys.h"

void delay(int period) {
    while (period--);
}

void test_command1() {  // test fork functionality
    int cnt = 0;
    if (fork() == 0) {
        fork();
        fork();
        while (cnt < 10) {
            printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++);  // address should be the same across tasks, but the cnt should be increased indepndently
            delay(1000000);
        }
        exit(0);  // all childs exit
    }
}

void test_command2() {  // test page fault
    if (fork() == 0) {
        int* a = 0x0;        // a non-mapped address.
        printf("%d\n", *a);  // trigger simple page fault, child will die here.
    }
}

void test_command3() {                                          // test page reclaim.
    printf("Remaining page frames : %d\n", remain_page_num());  // get number of remaining page frames from kernel by system call.
}

void shell_input(char* cmd) {
    printf("\r# ");

    int idx = 0;
    cmd[0] = '\0';
    char read_buf[2];
    char c;
    while (1) {
        uart_read(read_buf, 1);
        c = read_buf[0];
        if (c == '\n') break;
        else {
            cmd[idx++] = c;
            cmd[idx] = '\0';
        }
        printf("\r# %s", cmd);
    }

    printf("\n");
}

void shell_controller(char* cmd) {
    if (!strcmp(cmd, "")) {
        return;
    }
    else if (!strcmp(cmd, "help")) {
        printf("help: print all available commands\r\n");
        printf("hello: print Hello World!\r\n");
    }
    else if (!strcmp(cmd, "hello")) {
        printf("Hello World!\r\n");
    }
    else if (!strcmp(cmd, "test1")) {
        test_command1();
    }
    else if (!strcmp(cmd, "test2")) {
        test_command2();
    }
    else if (!strcmp(cmd, "test3")) {
        test_command3();
    }
    else {
        printf("shell: command not found: %s\r\n", cmd);
    }
}
