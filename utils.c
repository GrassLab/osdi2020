#include "utils.h"
#include "uart.h"

int strcmp(const char *s1,const char *s2){
    int b=0;
    while(*s2&&(*s1==*s2)){
        s1++;
        s2++;
    }
    if((*s2=='\0')&&!*s1)   return 1;
    else return 0;
}

void print(const char *s){
    while(*s!='\0'){
        uart_write(*s);
        s++;
    }
}

void init(){
    char c=8;
    uart_write(c);
    print("Hello RPI3\n\r#");
}

