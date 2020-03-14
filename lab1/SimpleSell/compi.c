#include <stdio.h>
#include <stdbool.h>
#include "simshell.h"
#include <string.h>

int main(void) 
{

    simshell simsl;
    char rx_buffer[500];
    int return_length;
    int value;
    if(!set_up_options(&simsl)) {
        printf("Can't initialize simple shell");     
        return 0;
    }

    printf("%s","# ");
    while (read_write_user_input(&simsl)) {
        memset(rx_buffer,'\0' ,500);
        do{
            value = read_pi_return(&simsl);
            if (value == -1) {
                continue;
            }
            else if(value == 1) {
                strcat(rx_buffer, simsl.rx_buffer);
                int i;
                for (i = 0 ; i < 499; i++) {
                    if(rx_buffer[i] == '#')
                        rx_buffer[i] = '\n';
                }
                break;
            }
            strcat(rx_buffer, simsl.rx_buffer);
        }while(1);
        if (value == 1){
            printf("%s\n", rx_buffer);

            printf("%s", "# ");
        }
    }
    return -1;
}
