#include "utils.h"
#include "uart.h"

char buff[10];
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

void print_hex(int num){
    int i;
	for(i=0;num!=0;i++){
        buff[i]=(num%16<10)?(num%16+'0'):(num%16-10+'A');
        num/=16;
    }
    print("0x");
	while(i>0){
        --i;
        uart_write(buff[i]);
    }
}

void print_dec(unsigned int num){
	int i;
	for(i=0;num!=0;i++){
        buff[i]=num%10+'0';
        num/=10;
    }
    while(i>0){
        --i;
        uart_write(buff[i]);
    }
}

void init(){
    char c=8;
    uart_write(c);
    print("Hello RPI3\n\r#");
}

