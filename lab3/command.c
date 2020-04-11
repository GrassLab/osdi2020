#include "string.h"
#include "uart.h"

#define TMP_KERNEL_ADDR 0x600000
#define PM_RSTC ((volatile unsigned int *)(MMIO_BASE + 0x0010001c))
#define PM_RSTS ((volatile unsigned int *)(MMIO_BASE + 0x00100020))
#define PM_WDOG ((volatile unsigned int *)(MMIO_BASE + 0x00100024))
#define PM_WDOG_MAGIC 0x5a000000
#define PM_RSTC_FULLRST 0x00000020

extern char __bss_start[];
extern char __bss_end[];

void reboot() {
  unsigned int r;
  // trigger a restart by instructing the GPU to boot from partition 0
  r = *PM_RSTS;
  r &= ~0xfffffaaa;
  *PM_RSTS = PM_WDOG_MAGIC | r; // boot from partition 0
  *PM_WDOG = PM_WDOG_MAGIC | 10;
  *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;

  while (1)
    ;
}

void loadimg() {
  unsigned long k_addr = 0; // 64bit
  void *load_address;
  uart_puts("bss : 0x");
  uart_hex_64((unsigned long int)__bss_start);
  uart_puts("\nbss_end : 0x");
  uart_hex_64((unsigned long int)__bss_end);
  uart_puts("\ngive me kernel address : (0x)");
  // read address
  for (int i = 0; i < 8; i++) {
    char get = uart_getc();
    uart_send(get);
    if (get == '\n') {
      break;
    } else {
      k_addr <<= 4;
      int in = 0;
      if (get >= '0' && get <= '9')
        in = get - '0';
      else if (get >= 'a' && get <= 'f')
        in = get - 'a' + 10;
      else if (get >= 'A' && get <= 'F')
        in = get - 'A' + 10;
      else {
        uart_puts("input error");
        return;
      }
      k_addr += in;
    }
  }
  uart_puts("\nyour input addrs = 0x");
  uart_hex_64(k_addr);
  load_address = (void *)k_addr;
  uart_puts("\ngive me kernel size : ");

  // read size
  unsigned long size = 0;
  for (int i = 0; i < 8; i++) {
    char get = uart_getc();
    uart_send(get);
    if (get == '\n') {
      break;
    } else {
      int in = 0;
      if (get >= '0' && get <= '9') {
        size = size * 10;
        in = get - '0';
        size += in;
      } else {
        uart_puts("input error\n");
        return;
      }
    }
  }
  uart_puts("\nkernel size : ");
  itoa(size);

  // is k_addr modify bss ?
  uart_send('\n');
  if ((k_addr >= (unsigned long)__bss_start &&
       k_addr <= (unsigned long)__bss_end) ||
      ((k_addr + size) >= (unsigned long)__bss_start &&
       (k_addr + size) <= (unsigned long)__bss_end)) {
    uart_puts("in bss\n");
    load_address = (void *)TMP_KERNEL_ADDR;
    volatile char *add = load_address;
    for (int i = 0; i < size; i++) {
      // read img
      *add = uart_img();
      add++;
    }
    // jmp to img
    uart_puts("jmp\n");
    asm volatile("mov sp, %0" ::"r"(load_address));
    ((void (*)(void))(load_address))();
  } else {
    // cant modify
    uart_puts("not in bss, give me img\n");
    volatile char *add = load_address;
    for (int i = 0; i < size; i++) {
      // read img
      *add = uart_img();
      add++;
    }
    // jmp to img
    uart_puts("jmp\n");
    asm volatile("mov sp, %0" ::"r"(load_address));
    ((void (*)(void))(load_address))();
  }
}

unsigned int get_system_frequency() {
  unsigned int res = 0;
  asm volatile("MRS %[result], CNTFRQ_EL0" : [result] "=r"(res));
  return res;
}

unsigned int get_system_count() {
  unsigned int res = 0;
  asm volatile("MRS %[result],  CNTPCT_EL0" : [result] "=r"(res));
  return res;
}