#include "user_lib.h"
// #include "shell.h"

void demo_lab7_ele2() {
    int a = open("hello", O_CREAT);
    int b = open("world", O_CREAT);
    int c = open("hello", O_CREAT);
    int d = open("world", O_CREAT);
    printf("fd: %d\n", a);
    printf("fd: %d\n", b);
    printf("fd: %d\n", c);
    printf("fd: %d\n", d);
}

int main() {
    // while (1) {
    //     char cmd[64];
    //     shell_input(cmd);
    //     shell_controller(cmd);
    // }
    demo_lab7_ele2();
}