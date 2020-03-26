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

void help(void) {
  mini_uart_puts("hello: print Hello World!" EOL);
  mini_uart_puts("help: help" EOL);
  mini_uart_puts("reboot: reboot rpi3" EOL);
  mini_uart_puts("timestamp: get current timestamp" EOL);
}

void hello(void) {
  mini_uart_puts("Hello World!" EOL);
}

void lshw(void) {
  char buf[32];
  mini_uart_puts("Board revision: ");
  mini_uart_puts("0x");
  mini_uart_puts(uitos_generic(get_board_revision(), 16, buf));
  mini_uart_puts(EOL);

  mini_uart_puts("VC core address: ");
  mini_uart_puts("0x");
  mini_uart_puts(uitos_generic(get_vc_memory(), 16, buf));
  mini_uart_puts(EOL);
}

void timestamp(void) {
  uint64_t count, frequency;
  asm("mrs %0, cntpct_el0" : "=r"(count));
  asm("mrs %0, cntfrq_el0" : "=r"(frequency));
  uint64_t time_int = count / frequency;
  uint64_t time_fra = (count * 1000000 / frequency) % 1000000;

  char buf[32];
  mini_uart_puts("[");
  mini_uart_puts(uitos(time_int, buf));
  mini_uart_puts(".");
  mini_uart_puts(uitos(time_fra, buf));
  mini_uart_puts("]" EOL);
}

void reboot(void) {
  // full reset
  *PM_RSTC = PM_PASSWORD | 0x20;
  mini_uart_puts("Reboot..." EOL);
  while (true);
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
      } else if (!strcmp(cmd, "hello")) {
        hello();
      } else if (!strcmp(cmd, "lshw")) {
        lshw();
      } else if (!strcmp(cmd, "reboot")) {
        reboot();
      } else if (!strcmp(cmd, "timestamp")) {
        timestamp();
      } else {
        mini_uart_puts("Error: command ");
        mini_uart_puts(cmd);
        mini_uart_puts(" not found, try <help>" EOL);
      }
    }
  }
}
