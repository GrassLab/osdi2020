#include "user_lib.h"
// #include "shell.h"

void demo_lab7_ele2() {
    int a = open("hello", O_CREAT);
    printf("fd: %d\n", a);
    close(a);
}

int main() {
    // while (1) {
    //     char cmd[64];
    //     shell_input(cmd);
    //     shell_controller(cmd);
    // }
    demo_lab7_ele2();
}