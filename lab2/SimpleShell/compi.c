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
        if (strcmp(rx_buffer, "kernel") == 0) {
            send_kernel(&simsl);
        }
        else if (strcmp(rx_buffer, "address") == 0) {
            read_vc_address(&simsl);
        } 
        else if (strcmp(rx_buffer, "revision") == 0) {
            read_revision(&simsl);
        }
        else{
            printf("%s\n", rx_buffer);
        }
        printf("%s", "# ");
    }
    return -1;
}