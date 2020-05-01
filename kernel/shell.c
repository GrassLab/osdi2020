#include "ioutil.h"
#include "mailbox.h"
#include "mini_uart.h"
#include "shell.h"
#include "string.h"
#include "utils.h"

void help(void) {
  mini_uart_puts("hello: print Hello World!" EOL);
  mini_uart_puts("help: help" EOL);
  mini_uart_puts("loadimg: load kernel image to specific location" EOL);
  mini_uart_puts("reboot: reboot rpi3" EOL);
  mini_uart_puts("timestamp: get current timestamp" EOL);
}

void hello(void) {
  mini_uart_puts("Hello World!" EOL);
}

void loadimg(void) {
  const uint64_t default_base = 0x90000;
  mini_uart_puts("Start Loading kernel image..." EOL);
  printf("Please input kernel load address (default: %#x): ", default_base);

  char buf[32];
  mini_uart_gets(buf);
  uint64_t base = strlen(buf) == 0 ? default_base : stoui(buf, 16);

  uint32_t size;
  uint16_t checksum;
  mini_uart_puts("Please send kernel image from UART now..." EOL);
load:
  mini_uart_getn(false, (uint8_t *)&size, sizeof(size));
  mini_uart_getn(false, (uint8_t *)&checksum, sizeof(checksum));

  uint8_t *begin = base, end = begin + size;
  if ((end >= __text_start && end < __text_end) ||
      (begin >= __text_start && begin < __text_end) ||
      (begin <= __text_start && end > __text_end)) {
    mini_uart_puts("The image will overlap with loader code." EOL);
    mini_uart_puts("Default load address will be used." EOL);
    base = default_base;
  }

  printf("Kernel Image Size: %u, Load Addr: %#x" EOL, size, base);

  uint32_t chk = 0;
  for (uint32_t i = 0; i < size; ++i) {
    uint8_t byte = mini_uart_getc(false);
    chk = (chk + byte) % 65536;
    *((uint8_t *)base + i) = byte;
  }

  if (chk == checksum) {
    ((void (*)(void))base)();
  } else {
    mini_uart_puts("The image is corrupted, please retry..." EOL);
    goto load;
  }
}

void lshw(void) {
  printf("Board revision: %#x" EOL, get_board_revision());

  uint64_t data = get_vc_memory();
  uint32_t base = data & 0xffffffff;
  uint32_t size = data >> 32;
  printf("VC core address: %#x-%#x (%#x bytes)" EOL, base, base + size - 1, size);
}

void timestamp(void) {
  uint64_t count, frequency;
  asm("mrs %0, cntpct_el0" : "=r"(count));
  asm("mrs %0, cntfrq_el0" : "=r"(frequency));
  uint64_t time_int = count / frequency;
  uint64_t time_fra = (count * 1000000 / frequency) % 1000000;

  printf("[%u.%u]" EOL, time_int, time_fra);
}

void reboot(void) {
  // full reset
  *PM_RSTC = PM_PASSWORD | 0x20;
  mini_uart_puts("Reboot..." EOL);
  while (true) {}
}

void shell(void) {
  mini_uart_puts("               _ _       _          _ _ " EOL);
  mini_uart_puts("  ___  ___  __| (_)  ___| |__   ___| | |" EOL);
  mini_uart_puts(" / _ \\/ __|/ _` | | / __| '_ \\ / _ \\ | |" EOL);
  mini_uart_puts("| (_) \\__ \\ (_| | | \\__ \\ | | |  __/ | |" EOL);
  mini_uart_puts(" \\___/|___/\\__,_|_| |___/_| |_|\\___|_|_|" EOL);
  mini_uart_puts(EOL);

  while (true) {
    mini_uart_puts("# ");
    char buf[MAX_CMD_LEN];
    mini_uart_gets(buf);

    char *cmd = strtrim(buf);
    if (strlen(cmd) != 0) {
      if (!strcmp(cmd, "help")) {
        help();
      } else if (!strcmp(cmd, "exc")) {
        asm("svc #1");
      } else if (!strcmp(cmd, "hello")) {
        hello();
      } else if (!strcmp(cmd, "irq")) {
        asm("svc #2");
      } else if (!strcmp(cmd, "loadimg")) {
        loadimg();
      } else if (!strcmp(cmd, "lshw")) {
        lshw();
      } else if (!strcmp(cmd, "reboot")) {
        reboot();
      } else if (!strcmp(cmd, "timestamp")) {
        timestamp();
      } else {
        printf("Error: command %s not found, try <help>" EOL, cmd);
      }
    }
  }
}
