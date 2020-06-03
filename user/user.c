#include "user_lib.h"
// #include "shell.h"

void test1() {
    int token = obj_alloc_init(100);
    printf("%d\n", token);
    // while(1);
}

int main() {
    // while (1) {
    //     char cmd[64];
    //     shell_input(cmd);
    //     shell_controller(cmd);
    // }
    test1();
}