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

void reverse(char *x, char *y){
    int i=0, j=0;
    while(x[i] != '\0'){
        i++;
    }

    i--;

    while(i > -1){
        y[j++] = x[i--];
    }
    y[j] = '\0';
}

void itoa(int x, char *res){
    char buf[30];
    int i=0, j=0;

    while (x != 0){
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }
    buf[i] = '\0';

    reverse(buf, res);
}

void ftoa(float x, char *i_res, char *f_res){
    int i_part = (int)x;
    float f_part = x - i_part;
    itoa(i_part, i_res);
    f_part *= 100000;
    i_part = (int)f_part;
    itoa(i_part, f_res);
}
