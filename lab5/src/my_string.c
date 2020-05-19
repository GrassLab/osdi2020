#include "my_string.h"

int strcmp(const char *x, const char *y){
    while(*x == *y){
        x++;
        y++;
    }

    if ((*x == '\n' || *x == '\r' || *x == '\0') && (*y == '\n' || *y == '\r' || *y == '\0')){
        return 0;
    }

    else return 1;
}

char* strtok(char *x, const char *delim){
    // Work In Process
    static char *last;

    if (x == 0)
        x = last;

    for (int i=0; delim[i] != '\0'; i++){
        while (*x != '\0'){
            if (*x == delim[i])
                *x = '\0';
            x++;
        }
    }

    return x;
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

void itoa(long long x, char *res){
    if (x == 0) {
        res[0] = '0';
        res[1] = '\0';
        return;
    }
    char buf[30];
    int i=0, j=0;

    while (x != 0){
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }
    buf[i] = '\0';

    reverse(buf, res);
}

void unsign_itoa(unsigned long long x, char *res){
    if (x == 0) {
        res[0] = '0';
        res[1] = '\0';
        return;
    }
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
    unsign_itoa(i_part, i_res);
    f_part *= 1000000;
    i_part = (int)f_part;
    unsign_itoa(i_part, f_res);
}

void unsign_itohexa(unsigned long long x, char *res){
    if (x == 0) {
        res[0] = '0';
        res[1] = '\0';
        return;
    }
    char buf[20];
    int i=0;
    while(x != 0){
        buf[i++] = (x % 16) > 9 ? 'a' + (x % 16) - 10 : '0' + (x % 16);
        x /= 16;
    }
    buf[i] = '\0';

    reverse(buf, res);
}

int atoi(char *s){
    int res=0;
    res += *s - '0';
    s++;
    while(*s != '\0'){
        res *= 10;
        res += *s - '0';
        s++;
    }

    return res;
}

int hexatoi(char *s){
    int res=0;
    res += *s >='0' && *s <='9' ? *s - '0' : *s - 'a' + 10;
    s++;
    while(*s != '\0'){
        res *= 16;
        res += *s >='0' && *s <='9' ? *s - '0' : *s - 'a' + 10;
        s++;
    }

    return res;
}
