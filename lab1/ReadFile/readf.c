#include <stdio.h>
#include <stdbool.h>
#include "simshell.h"
#include <string.h>

int main(void) 
{
    FILE *fptr;
    char buffer[200];
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
    while(feof(fptr) == 0) {
        fscanf(fptr, "%s", buffer); 
        write(simsl.utf, buffer, strlen(buffer));
        write(simsl.utf, "\n", 1);
    }

    return 0;
}