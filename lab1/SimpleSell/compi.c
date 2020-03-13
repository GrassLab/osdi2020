#include <stdio.h>
#include <stdbool.h>
#include "simshell.h"

int main(void) 
{

    simshell simsl;
    if(!set_up_options(&simsl)) {
        printf("Can't initialize simple shell");     
        return 0;
    }
    while (read_write_user_input(&simsl)) {
	
    }
    return -1;
}
