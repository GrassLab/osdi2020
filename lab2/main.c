#include "uart.h"
#include "shell.h"
#include "printf.h"
#include "lfb.h"

#define BOOT_MSG "                                                               \n" \
                 "                                                               \n" \
                 "    )                                                      ____\n" \
                 " ( /(       (   (            (  (               (   (     |   /\n" \
                 " )\\())   (  )\\  )\\           )\\))(   '     (    )\\  )\\ )  |  /\n" \
                 "((_)\\   ))\\((_)((_) (       ((_)()\\ )  (   )(  ((_)(()/(  | /\n" \
                 " _((_) /((_)_   _   )\\      _(())\\_)() )\\ (()\\  _   ((_)) |/\n" \
                 "| || |(_)) | | | | ((_)  )  \\ \\((_)/ /((_) ((_)| |  _| | (\n" \
                 "| __ |/ -_)| | | |/ _ \\ /(   \\ \\/\\/ // _ \\| '_|| |/ _` | )\\\n" \
                 "|_||_|\\___||_| |_|\\___/(_))   \\_/\\_/ \\___/|_|  |_|\\__,_|((_)\n" \
                 "                                                                  \n"

int
main (void)
{
  // init stack guard. It should be random, but I'm lazy.
  __stack_chk_guard = (void *) 0xdeadbeef;
  uart_init ();
  lfb_init ();
  uart_puts (BOOT_MSG);
  shell_interactive ();
  return 0;
}
