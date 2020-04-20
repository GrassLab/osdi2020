#include "mailbox.h"
#include "mini_uart.h"
#include "shell.h"

int strcmp(const char *s1, const char *s2) {
  for (; *s1 != '\0' && *s1 == *s2; ++s1, ++s2);
  return *s1 - *s2;
}

uint32_t strlen(const char *s) {
  uint32_t len = 0;
  for (; s[len] != '\0'; ++len);
  return len;
}

int isspace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char *strtrim(char *s) {
  char *begin = s;
  for (; isspace(*begin); ++begin);
  char *end = begin + strlen(begin) - 1;
  for (; end > begin && isspace(*end); --end);
  *(end + 1) = '\0';
  return begin;
}

char *uitos_generic(uint64_t num, int base, char *buf) {
  const char *alphabet = "0123456789abcdef";
  char *cur = buf;
  uint8_t len = 0;

  do {
    *cur = alphabet[num % base];
    num /= base;
    ++cur;
    ++len;
  } while (num > 0);
  *cur = '\0';

  for (int i = 0; i < len / 2; ++i) {
    char tmp = buf[len - i - 1];
    buf[len - i - 1] = buf[i];
    buf[i] = tmp;
  }

  return buf;
}

char *uitos(uint64_t num, char *buf) {
  uitos_generic(num, 10, buf);
}

int32_t ctoi(char c) {
  if (c >= 'A' && c <= 'F') {
    return 10 + (c - 'A');
  } else if (c >= 'a' && c <= 'f') {
    return 10 + (c - 'a');
  } else if (c >= '0' && c <= '9') {
    return c - '0';
  } else {
    return -1;
  }
}

uint64_t pow(uint32_t base, uint32_t exponent) {
  uint64_t rst = 1;
  for (uint32_t i = 0; i < exponent; ++i) {
    rst *= base;
  }
  return rst;
}

uint64_t stoui(const char *s, int base) {
  uint64_t num = 0;
  for (int32_t i = strlen(s) - 1, j = 0; i >= 0; --i, ++j) {
    int32_t val = ctoi(s[i]);
    if (val < 0) {
      return num;
    } else {
      num += val * pow(base, j);
    }
  }
  return num;
}

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
