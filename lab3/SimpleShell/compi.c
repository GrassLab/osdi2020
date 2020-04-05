#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "simshell.h"
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
        read_line(&simsl, rx_buffer);
        if (strcmp(rx_buffer, "exc") == 0) {
            read_exc_return(&simsl);
        }
        else if(strcmp(rx_buffer, "irq1") == 0) {
            printf("irq1\n");
            printf("777\n");
            while(1) {
                printf("77744\n");
                read_line(&simsl, rx_buffer);
                printf("%s\n", rx_buffer);
                printf("6666\n");
            }
        }
        else if(strcmp(rx_buffer, "irq") == 0) {
            unsigned int x ;
            printf("irq\n");
            while(read_line(&simsl, rx_buffer)) {
                printf("%s\n", rx_buffer);
            }
        }
        else {
            printf("%s\n", rx_buffer);
        }
        printf("%s", "# ");
    }
    return -1;
}