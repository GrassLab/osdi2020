#include "command.h"
#include "lfb.h"
#include "mbox.h"
#include "string.h"
#include "uart.h"

char *welcome = " \
░░░░░░░░░░░░░░░░▄░█▄░█▄▄▄░░\n \
█▀▀█░░█▀▀█░█▀▀█▄█▄█▄░█▄▄▄█░\n \
█▀▀█░░█▀▀█░█░░█░░▄█▄▄▄░░░░░\n \
█▀▀▀░░▀▀▀█░█░░█░▀▄▄▄█▄▄▄░░░\n \
█░▄▄▄▄█▄░█░▀▀▀▀░░█▄▄█▄▄█░░░\n \
█░░░▄▀█░░█░░░░░░░█▄▄█▄▄█░░░\n \
█░▄▀░▄█░▄█░░░░░░▄▀░░░░▄█░░░\n \
";

void compair(char *buf) {
  if (strcpy(buf, "hello")) {
    printf("Hello World!\n");
  } else if (strcpy(buf, "help")) {
    printf("help : print all  command\n");
    printf("hello : say hello\n");
    printf("timestamp : get time\n");
    printf("reboot : reboot rpi3 \n");
    printf("loadimg : loadimg you want \n");
    printf("version : show board revision \n");
    printf("vc_addr : show vc base address \n");
  } else if (strcpy(buf, "timestamp")) {
    long countertimer = get_system_count();
    long frequency = get_system_frequency();
    countertimer = countertimer * 1000;
    long result = countertimer / frequency;
    printf("%d ms\n", result);
  } else if (strcpy(buf, "reboot")) {
    reboot();
  } else if (strcpy(buf, "loadimg")) {
    loadimg();
  } else if (strcpy(buf, "version")) {
    board_revision();
  } else if (strcpy(buf, "vc_addr")) {
    vc_base_address();
  } else if (strcpy(buf, "exec")) {
  } else {
    uart_puts("unknow command ");
    uart_puts(buf);
    uart_puts(". \n");
  }
}

void main() {
  uart_init();
  // wait_cycles(10000);
  // lfb_init();
  // wait_cycles(10000);
  // // display a pixmap
  // lfb_showpicture();
  // wait_cycles(10000);

  printf(welcome);
  printf("# ");

  char commandbuf[100];
  clearbuf(commandbuf, 100);
  int count = 0;
  while (1) {
    char get = uart_getc();
    uart_send(get);
    if (get != '\n') {
      commandbuf[count] = get;
      count++;
    } else {
      commandbuf[count] = '\x00';
      compair(commandbuf);
      printf("# ");
      count = 0;
      clearbuf(commandbuf, 100);
    }
  }
}
