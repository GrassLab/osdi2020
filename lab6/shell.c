#include "shell.h"
#include "meta_macro.h"
#include "string_util.h"
#include "syscall.h"

const char * shell_command_list[] = {
  "hello",
  "help",
  "exit",
  "malloc",
  "free",
  0x0
};

const char * shell_command_descriptions[] = {
  "Print Hello World!",
  "Help",
  "Exit",
  "Malloc",
  "Free",
  0x0
};

int main(void)
{
  shell();
  return 0;
}

int (*shell_command_function_ptr[])(char *) = {
  shell_hello,
  shell_help,
  shell_exit,
  shell_malloc,
  shell_free,
  0x0
};

void shell(void)
{
  char string_buffer[0x100];

  while(1)
  {
    syscall_uart_puts(ANSI_RED "# " ANSI_RESET);
    syscall_uart_gets(string_buffer, '\n', 0x100 - 1);
    _shell_parser(string_buffer);
  }

  return;
}

void _shell_parser(char * string_buffer)
{
  /* remove newline */
  string_strip(string_buffer, '\n');

  if(string_length(string_buffer) == 0)
  {
    return;
  }

  /* Check commands */
  int command_idx = 0;
  for(; shell_command_list[command_idx] != 0x0; ++command_idx)
  {
    if(string_cmp(string_buffer, shell_command_list[command_idx], 999999))
    {
      (*shell_command_function_ptr[command_idx])(string_buffer);
      break;
    }
  }
  if(shell_command_list[command_idx] == 0x0)
  {
    syscall_uart_puts("Err: command ");
    syscall_uart_puts(string_buffer);
    syscall_uart_puts(" not found, try <help>\n");
  }
}

int shell_hello(char * string_buffer)
{
  UNUSED(string_buffer);
  syscall_uart_puts("Hello World!\n");
  return 0;
}

int shell_help(char * string_buffer)
{
  string_buffer[0] = '\0';
  for(int command_idx = 0; shell_command_list[command_idx] != 0x0; ++command_idx)
  {
    string_concat(string_buffer, shell_command_list[command_idx]);
    string_concat(string_buffer, ": ");
    string_concat(string_buffer, shell_command_descriptions[command_idx]);
    string_concat(string_buffer, "\n");
  }
  syscall_uart_puts(string_buffer);
  return 0;
}

int shell_exit(char * string_buffer)
{
  UNUSED(string_buffer);
  syscall_exit(0);
  return 0;
}

int shell_malloc(char * string_buffer)
{
  unsigned bytes;
  syscall_uart_puts("Enter order in hex > ");
  syscall_uart_gets(string_buffer, '\n', 0x100 - 1);
  string_strip(string_buffer, '\n');
  bytes = (unsigned)string_hex_char_to_longlong(string_buffer);
  syscall_malloc(bytes);
  return 0;
}

int shell_free(char * string_buffer)
{
  uint64_t * va;
  syscall_uart_puts("Enter VA in hex > ");
  syscall_uart_gets(string_buffer, '\n', 0x100 - 1);
  string_strip(string_buffer, '\n');
  va = (uint64_t *)string_hex_char_to_longlong(string_buffer);
  syscall_free(va);
  return 0;
}

