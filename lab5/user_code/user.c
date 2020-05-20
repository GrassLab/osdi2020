#include "user_lib.h"

int main()
{	
    call_sys_write("User Program:\n");
    
    while(5){
        char in = call_sys_read();
        call_sys_write_char(in);
    }
    return 0;
}