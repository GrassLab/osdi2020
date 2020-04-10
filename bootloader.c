#include "uart.h"
#include "bootloader.h"
#include "string.h"

void loadimg (unsigned long address, unsigned long img_size)
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
  uart_puts ("image size: ");
  uart_send_int ((int)img_size);
  uart_puts ("\n");

  // rebase tiny bootloader
  rebased_bootloader = address + img_size + 0x10000;
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

  
  uart_puts("Move bootloader done, start copy kernel\n");
loadimg_jmp(address, img_size);
  uart_send_hex(rebased_loadimg);
  asm volatile ("mov x0, %0;" \
                "mov x1, %1;" \
                "mov sp, %2;" \
                ::"r" (address), "r" (img_size), "r" ((rebased_end + 0x8000) & ~0xf));
  asm volatile("blr %0;" :: "r" (rebased_loadimg): "x0", "x1");

    void (*func)(void*, unsigned long) = (void (*)(void*, unsigned long))rebased_loadimg;
    func(address, img_size);

    uart_puts("no see");

  asm volatile ("mov x0, %0\n" "mov x1, %1\n" "mov sp, %2\n"
        "blr %3\n"::"r" (address), "r" (img_size), "r" ((rebased_end + 0x8000) & ~0xf),
        "r" (rebased_loadimg):"x0", "x1");


}


void loadimg_jmp (void *address, unsigned long img_size)
{
  // save image base
  char *buf = (char*)address;
  int count = img_size;

  uart_puts("Start copy kernel\n");
  uart_send_hex((int)address);
  uart_send_int((int)img_size);
  while(count--){
    // uart_getc()
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(*UART0_FR&0x10);
    /* read it and return */
    *buf = (char)(*UART0_DR);
    /* convert carrige return to newline */
    buf++;
  }
 uart_puts("Copy kernel done, jump to new kernel\n");

   asm volatile("mov x30, 0x80000; ret");
  ((void (*)(void)) (address)) ();
}
