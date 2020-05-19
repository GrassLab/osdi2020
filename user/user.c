#include "user_lib.h"

void delay(int period) {
    while (period--);
}

int main() {
    while(1) {
        printf("message from user %d %d\n", get_taskid(), remain_page_num());
        delay(100000000);
        exit(1);
    }
    return 0;
}