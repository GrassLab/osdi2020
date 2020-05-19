#include "user_lib.h"

void delay(int period) {
    while (period--);
}

int main() {
    while(1) {
        printf("message from user\n");
        delay(100000000);
    }
    return 0;
}