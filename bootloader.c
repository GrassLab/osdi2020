#include "uart.h"
#include "bootloader.h"
#include "string.h"

void
loadimg (unsigned long address, unsigned long img_size)
{
  extern void *__start_bootloader;
  extern void *__stop_bootloader;
  unsigned long __bootloader_size = (unsigned long)&__stop_bootloader - (unsigned long)&__start_bootloader;
  // unsigned long img_size;
  unsigned long rebased_bootloader;
  unsigned long rebased_end;
  unsigned long rebased_loadimg;
  char buf[0x20];
  uart_puts ("Load image into 0x");
  uart_send_hex (address);
  uart_puts ("\n");
  uart_puts ("[");
  uart_puts (buf);
  uart_puts ("] image size: 0x");
  uart_send_int (img_size);
  uart_puts ("\n");
  // rebase tiny bootloader
  rebased_bootloader = address + img_size;
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
        "blr %3\n"::"r" (address), "r" (img_size), "r" ((rebased_end + 0x8000) & ~0xf),
        "r" (rebased_loadimg):"x0", "x1");
  //((void (*)(void *, unsigned long, void *)) rebased_bootloader)((void *) address, img_size, rebased_bootloader + &__bootloader_size);
}

void uart_read(char *buf, unsigned long count){
    while(count--)
        *buf++ = uart_getc();
}

void loadimg_jmp (void *address, unsigned long img_size)
{
  // save image base
  uart_read ((char *) address, img_size);
  ((void (*)(void)) (address)) ();
}
