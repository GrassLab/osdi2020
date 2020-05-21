#include "uart.h"
#include "bootloader.h"
#include "string.h"
#include "syscall.h"

#define INPUT_BUFFER_SIZE 32

void loadimg()
{
  extern void *__start_bootloader;
  extern void *__stop_bootloader;
  unsigned long __bootloader_size = (unsigned long)&__stop_bootloader - (unsigned long)&__start_bootloader;

  unsigned long rebased_bootloader;
  unsigned long rebased_end;
  unsigned long rebased_loadimg;

  // read parameter
  uart_puts("Input kernel size: ");
  char cmd[INPUT_BUFFER_SIZE];
  memset(cmd, 0, INPUT_BUFFER_SIZE);

  uart_gets(cmd, INPUT_BUFFER_SIZE);
  unsigned long image_size = atoi(cmd);
  uart_puts("Kernel size is: ");
  uart_send_int(image_size);
  uart_send('\n');

  uart_puts("Input Kernel load address: ");
  memset(cmd, 0, INPUT_BUFFER_SIZE);
  uart_gets(cmd, INPUT_BUFFER_SIZE);

  unsigned long address = atoi(cmd);
  uart_puts("Kernel load address is: ");
  uart_send_hex(address);
  uart_send('\n');

  // copy_kernel_and_load_images((void *)(long)address, image_size);
  // load_images((char *)(long)address, image_size);
  char buf[0x20];
  uart_puts("Load image into 0x");
  uart_send_hex(address);
  uart_puts("\n");
  uart_puts("image size: ");
  uart_send_int((int)image_size);
  uart_puts("\n");

  // rebase tiny bootloader
  rebased_bootloader = address + image_size + 0x2000;
  rebased_end = rebased_bootloader + __bootloader_size;

  /*
  if ((unsigned long)&rebased_bootloader < address + image_size)
    rebased_bootloader = (unsigned long)(address + image_size);
    */

  // place new bootloader
  memcpy((char *)rebased_bootloader, &__start_bootloader,
         __bootloader_size);

  // call rebased bootloader
  rebased_loadimg =
      rebased_bootloader + ((unsigned long)&loadimg_jmp - (unsigned long)&__start_bootloader);

  uart_puts("Move bootloader done, start copy kernel\n");

  uart_send_hex(rebased_loadimg);

  //loadimg_jmp(address, image_size);

  /*
  asm volatile("mov x0, %0;"
               "mov x1, %1;"
               "mov sp, %2;" ::"r"(address),
               "r"(image_size), "r"((rebased_end + 0x8000) & ~0xf));
  asm volatile("blr %0;" ::"r"(rebased_loadimg)
               : "x0", "x1");
               */

  //asm volatile("mov sp, %0" ::"r"((rebased_end + 0x8000) & ~0xf));

  void (*func)(void *, unsigned long) = (void (*)(void *, unsigned long))rebased_loadimg;
  //uart_send_hex((unsigned int)func);
  func((char *)address, image_size);

  uart_puts("no see");

  /*
  asm volatile("mov x0, %0\n"
               "mov x1, %1\n"
               "mov sp, %2\n"
               "blr %3\n" ::"r"(address),
               "r"(image_size), "r"((rebased_end + 0x8000) & ~0xf),
               "r"(rebased_loadimg)
               : "x0", "x1");
               */
}

void loadimg_jmp(void *address, unsigned long image_size)
{
  // save image base
  char *buf = (char *)address;
  unsigned long count = image_size;

  while (count--)
  {
    *buf = uart_recv();
    buf++;
  }

  asm volatile("br %0"
               : "=r"((unsigned long int *)address));

  //asm volatile("mov x30, 0x80000; ret");
  ((void (*)(void))(address))();
}
