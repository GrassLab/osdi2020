#include "uart.h"

char buff[50];
int strcmp(const char *s1,const char *s2);
void print(const char *s);
void init();


void main(){
	uart_init();
	init();
	char c;
	int count;
	int i;
	while(1){
		c=uart_read();
		if(c==8){
			if(count!=0){
				buff[count]=' ';
				count--;
				//delete?
			}
		}
		else if(c==10||c==13){
			uart_write('\n');
			uart_write('\r');
			if(strcmp(buff,"help")){
				print("hello : print Hello World!\n\r");
			}
			else if(strcmp(buff,"hello")){
				print("Hello World!\n\r");
			}
			else print("Err, command not found, try <help>.\n\r");
			for(i=0;i<50;i++)	buff[i]='\0';
			count=0;
			uart_write('#');
		}
		else{
			buff[count]=c;
			count++;
			uart_write(c);
		}
	}
}

int strcmp(const char *s1,const char *s2){
	int b=0;
	while(*s2&&(*s1==*s2)){
		s1++;
		s2++;
	}
	if((*s2=='\0')&&!*s1)	return 1;
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
