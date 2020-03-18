#include <stdio.h>
#include <stdbool.h>
#include "simshell.h"
#include <string.h>

int main(void) 
{
    FILE *fptr;
    char buffer[200];
    char rx_buffer[500];
    int value;

    fptr = fopen("./test.txt","r");
    if (fptr == NULL) {
        printf("Error");
        return -1;
    }
    simshell simsl;
    if(!set_up_options(&simsl)) {
        printf("Can't initialize simple shell");     
        return -1;
    }
    if (simsl.utf == -1) {
        printf("Fail to read user input\n");
	    return -1;
    }
    printf("%s","# ");
    while(feof(fptr) == 0) {
        memset(rx_buffer,'\0' ,500);
        fscanf(fptr, "%s", buffer); 
        printf("%s\n",buffer);
        write(simsl.utf, buffer, strlen(buffer));
        write(simsl.utf, "\n", 1);
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
            printf("%s", rx_buffer);
            printf("%s", "# ");
        }
    }

    return 0;
}