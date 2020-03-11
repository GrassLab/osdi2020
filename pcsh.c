#include "uart.h"

#define INPUT_BUFFER_SIZE 2048

int gets(char *buf, int buf_size){
    int i = 0;
    char c;

    do{
        c = uart_getc();
        c = c=='\r'?'\n':c;
        buf[i++] = c;

        // ensure users can see what they type
        uart_send(c);
    }while(c != '\n' && i < buf_size);
    
    if(i == buf_size)
        return -1;

    return 0;
}

int print(char *s){
    uart_puts(s);
}

int sh_default_command(char *cmd){


}

int symbol(){
    uart_send('>');
}



int pcsh(){
    
    // main loop
    // input, output

    while(1){
        // get command
        char cmd[INPUT_BUFFER_SIZE];

        symbol();
        gets(cmd, INPUT_BUFFER_SIZE);


        print(cmd);
        


        // default command
        sh_default_command(cmd);



        // other program

    }

    return 0;

}
