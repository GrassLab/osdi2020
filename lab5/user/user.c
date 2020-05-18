#include "user.h"
#include "user_sys.h"


void loop(char* str)
{
    char buf[2] = {""};
    while (1){
        for (int i = 0; i < 5; i++){
            buf[0] = str[i];
            call_sys_write(buf);
        }
    }
}



void user_process() {
  call_sys_write("** user process executed **");
  loop("hello");
  /* while(1) {} */
  /* call_sys_exit(); */
}
