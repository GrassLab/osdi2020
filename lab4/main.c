#include "command.h"
#include "irq.h"
#include "lfb.h"
#include "mbox.h"
#include "string.h"
#include "task.h"
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
void t1() {
  while (1) {
    uart_puts("1...\n");
    wait_cycles(1000000);
    schedule();
  }
}

void t2() {
  while (1) {
    uart_puts("2...\n");
    wait_cycles(1000000);
    schedule();
  }
}

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
    asm volatile("mov x0, #1");
    asm volatile("svc #0");
  } else if (strcpy(buf, "reboot")) {
    reboot();
  } else if (strcpy(buf, "loadimg")) {
    loadimg();
  } else if (strcpy(buf, "version")) {
    board_revision();
  } else if (strcpy(buf, "exec")) {
    execpt();
  } else if (strcpy(buf, "vc_addr")) {
    vc_base_address();
  } else if (strcpy(buf, "irq")) {
    irq();
  } else if (strcpy(buf, "lv")) {
    ellevel();
  } else if (strcpy(buf, "scs")) {
    task_init();
    privilege_task_create((unsigned long)&t1);
    privilege_task_create((unsigned long)&t2);
    schedule();
  } else {
    printf("unknow command %s\n", buf);
  }
  return;
}
void main() {
  uart_init();
  // wait_cycles(10000);
  // lfb_init();
  // wait_cycles(10000);
  // // display a pixmap
  // lfb_showpicture();
  // wait_cycles(10000);

  uart_puts(welcome);
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
