#include "uart.h"
#include "mystd.h"

#define CMDMAX 32

void main()
{
    uart_init();
    uart_puts("Hello RPI3!\n");
    char command[CMDMAX];
    while(1){
      uart_puts("# ");
      uart_gets(command);
      if (strcmp(command, "help"))
          uart_puts("help: help\n"
                    "hello: print Hello World!\n"
                    "timestamp: get current timestamp\n"
                    "reboot: reboot rpi3\n");
      else if (strcmp(command, "hello"))
          uart_puts("Hello World!\n");
      else if (strcmp(command, "timestamp")){
          uart_puts("[");
          uart_double(get_time());
          uart_puts("]\n");
      }
      else if (strcmp(command, "reboot")){
          uart_puts("reboot...\n");
          reset(100);
      }
      else if (*command)
          uart_puts("error: command not found,  try <help>\n");
    }
}
