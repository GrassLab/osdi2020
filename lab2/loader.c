#include "loader.h"
#include "libc.h"
#include "uart.h"

extern int __start;
extern int __end;

void loadimg(unsigned int bootloader_end) {
  uart_println("Start loading kernel image...");
  uart_println("Please input kernel load address(defualt: 0x80000):");
  uart_println("Please send kernel image from UART now:");
  /* The kernel image must have following protocal */
  /* recieve load address */
  /* recieve kernel size */
  /* start recieve kerenl_image based on kernel size */

  /* default: UPLOAD_ADDRESS defined in loader.h */
  void *load_address = (void *)UPLOAD_ADDRESS;

  void *new_addr = (void *)uart_getul();
  if (new_addr != 0) {
    load_address = new_addr;
  }

  /* kernel size */
  /* (e.g. 4000\r) */
  int kernel_size = uart_getint();

  uart_println("Kernel Image Size: %d Load addr: %x", kernel_size,
               load_address);

  /* checking the kernel has overlay the bootloader */
  /* if true then adjust the bootloader to lower address, otherwise coninue */
  unsigned int align_size = ceil((float)kernel_size / 4096) * 4096;
  unsigned int new_end = (unsigned int)load_address + align_size;

  if (new_end > BOOT_ADDR && (unsigned int)load_address < bootloader_end) {
    uart_println("\033[0;31m[ERR]\033[0m you cannot set the location @ 0x%x\r\n"
                 "  overlay the bootloader [0x%x - 0x%x]\r\n"
                 "  with your image        [0x%x - 0x%x]\r\n"
                 "please resend to other address",
                 load_address, BOOT_ADDR, bootloader_end, load_address,
                 new_end);
    goto EAT_KERNEL;
  }

  if ((unsigned int)new_addr % 4096 != 0) {
    uart_println(
        "\033[0;31m[ERR]\033[0m you should align the address to 4096 bytes\r\n"
        "that is the recommended address: 0x%x\r\n"
        "(e.g. the next align of 0x80000 is 0x81000)"
        "please resend to other address",
        ceil((float)(unsigned int)new_addr / 4096) * 4096);
    goto EAT_KERNEL;
  }

  /* Data ack */
  {
    int u = uart_getint();
    if (u != 1) {
      goto PROTOCAL_ERROR;
    }
  }

  /* starting recieve kernel image */
  volatile char *kernel_addr = load_address;
  while (kernel_size--) {
    *kernel_addr++ = uart_getc();
  }

  kentry_t kentry = (kentry_t)load_address;

  uart_println("adjust stack pointer to %x", load_address);
  asm volatile("mov sp, %0" ::"r"(load_address));

  uart_puts("Successfully loaded, executing uploaded kernel...\r\n");

  kentry();

PROTOCAL_ERROR:
  uart_println("[ERR] loading image fault(protocal)");
  return;
EAT_KERNEL:
  {
    int u = uart_getint();
    if (u != 1) {
      goto PROTOCAL_ERROR;
    }
  }

  while (kernel_size--) {
    volatile char c = uart_getc();
  }
  return;
}
