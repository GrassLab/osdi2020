#include "uart.h"
#include "bootloader.h"
#include "string.h"

#define INPUT_BUFFER_SIZE 64

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
  char *ptr = cmd;
  unsigned long image_size = 0;
  while(*ptr != '\0'){
        image_size *= 10;
        image_size += (*ptr - '0');\
        ptr++;
  }
  
  uart_puts("Kernel size is: ");
  uart_send_hex(image_size);
  uart_send('\n');

  uart_puts("Input Kernel load address: ");
  memset(cmd, 0, INPUT_BUFFER_SIZE);
  uart_gets(cmd, INPUT_BUFFER_SIZE);

  unsigned long address = 0;
  ptr = cmd;
  while(*ptr != '\0'){
        address *= 10;
        address += (*ptr - '0');\
        ptr++;
  }
  uart_puts("Kernel load address is: ");
  uart_send_hex(address);
  uart_send('\n');

  uart_puts("Load image into 0x");
  uart_send_hex(address);
  uart_puts("\n");
  uart_puts("image size: ");
  uart_send_hex((int)image_size);
  uart_puts("\n");


  // rebase tiny bootloader
  rebased_bootloader = address + image_size;
  rebased_end = rebased_bootloader + __bootloader_size;
  if ((unsigned long) &__stop_bootloader > rebased_end)
    rebased_bootloader = (unsigned long) &__stop_bootloader;

  // place new bootloader
  memcpy ((char *) rebased_bootloader, &__start_bootloader,
	  __bootloader_size);

  // call rebased bootloader
  rebased_loadimg =
    rebased_bootloader + ((unsigned long) &loadimg_jmp -
			  (unsigned long) &__start_bootloader);
  asm volatile ("mov x0, %0\n" "mov x1, %1\n" "mov sp, %2\n"
		"blr %3\n"::"r" (address), "r" (image_size),
		"r" ((rebased_end + 0x8000) & ~0xf),
		"r" (rebased_loadimg):"x0", "x1");
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
  ((void (*)(void))(address))();
}
