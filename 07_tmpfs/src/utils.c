#include "utils.h"

int strcmp(const char* a, const char* b) {
    int cur = 0;
    while ((a[cur] != 0) || (b[cur] != 0)) {
        if (a[cur] != b[cur]) {
            return cur;
        }
        cur ++;
    }
    return -1;
}

void strcpy(char* a, const char* b) {
    int i = 0;
    while (b[i] != '\0') {
        a[i] = b[i];
        ++ i;
    }
    a[i] = b[i];
}
