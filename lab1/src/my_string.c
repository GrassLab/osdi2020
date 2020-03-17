#include "my_string.h"

int strcmp(char *x, char *y){
    while(*x == *y){
        x++;
        y++;
    }

    if ((*x == '\n' || *x == '\r' || *x == '\0') && (*y == '\n' || *y == '\r' || *y == '\0')){
        return 0;
    }

    else return 1;
}
