#include "user_lib.h"

void delay(int period) {
    while (period--);
}

int main() {  // test fork functionality
    int cnt = 0;
    if (fork() == 0) {
        fork();
        fork();
        while (cnt < 10) {
            printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++);  // address should be the same across tasks, but the cnt should be increased indepndently
            delay(100000);
        }
        exit(0);  // all childs exit
    }
}

// int main() {
//     while(1) {
//         printf("message from user %d %d\n", get_taskid(), remain_page_num());
//         delay(100000000);
//         exit(1);
//     }
//     return 0;
// }