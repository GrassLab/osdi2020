//#include "user_lib.h"

void main(){
        char buffer[20]="this is a test\n";
        buffer[16]='\0';
  
        int size=0;
        size = uart_write(buffer,sizeof(buffer));
}
