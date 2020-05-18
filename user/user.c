#include "user_lib.h"

void delay(int period) {
    while (period--);
}

int main() {
    while(1) {
        printf("test test");
        delay(100000000);
    }
    return 0;
}