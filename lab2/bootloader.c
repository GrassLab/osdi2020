#include "bootloader.h"
#include "uart.h"
#include "meta_macro.h"
#include "string_util.h"

int main(int argc, char ** argv)
{
  UNUSED(argc);
  UNUSED(argv);
  bootloader_shell();
}

const char * bootloader_shell_command_list[] = {
  "help",
  0x0
};

const char * bootloader_shell_command_descriptions[] = {
  "Help",
  0x0
};

int (*bootloader_shell_command_function_ptr[])(char *) = {
  bootloader_shell_help,
  0x0
};


const char boot_figlet[] =
ANSI_GREEN
"    ___       ___       ___       ___\n"
"   /\\  \\     /\\  \\     /\\  \\     /\\  \\\n"
"  /::\\  \\   /::\\  \\   /::\\  \\    \\:\\  \\\n"
" /::\\:\\__\\ /:/\\:\\__\\ /:/\\:\\__\\   /::\\__\\\n"
" \\:\\::/  / \\:\\/:/  / \\:\\/:/  /  /:/\\/__/\n"
"  \\::/  /   \\::/  /   \\::/  /   \\/__/\n"
"   \\/__/     \\/__/     \\/__/\n";

const char loader_figlet[] =
"    ___       ___       ___       ___       ___       ___\n"
"   /\\__\\     /\\  \\     /\\  \\     /\\  \\     /\\  \\     /\\  \\\n"
"  /:/  /    /::\\  \\   /::\\  \\   /::\\  \\   /::\\  \\   /::\\  \\\n"
" /:/__/    /:/\\:\\__\\ /::\\:\\__\\ /:/\\:\\__\\ /::\\:\\__\\ /::\\:\\__\\\n"
" \\:\\  \\    \\:\\/:/  / \\/\\::/  / \\:\\/:/  / \\:\\:\\/  / \\;:::/  /\n"
"  \\:\\__\\    \\::/  /    /:/  /   \\::/  /   \\:\\/  /   |:\\/__/\n"
"   \\/__/     \\/__/     \\/__/     \\/__/     \\/__/     \\|__|\n"
ANSI_RESET;

void bootloader_shell(void)
{
  char string_buffer[0x1000];

  uart_init();

	/* Show boot message */
  uart_puts(boot_figlet);
  uart_puts(loader_figlet);

  while(1)
  {
    uart_puts(ANSI_RED "# " ANSI_RESET);
    uart_gets(string_buffer, '\n', 0x1000 - 1);
    bootloader_shell_parser(string_buffer);
  }

  return;
}

void bootloader_shell_parser(char * string_buffer)
{
  /* remove newline */
  string_strip(string_buffer, '\n');

  if(string_length(string_buffer) == 0)
  {
    return;
  }

  /* Check commands */
  int command_idx = 0;
  for(; bootloader_shell_command_list[command_idx] != 0x0; ++command_idx)
  {
    if(string_cmp(string_buffer, bootloader_shell_command_list[command_idx]))
    {
      (*bootloader_shell_command_function_ptr[command_idx])(string_buffer);
      break;
    }
  }
  if(bootloader_shell_command_list[command_idx] == 0x0)
  {
    uart_puts("Err: command ");
    uart_puts(string_buffer);
    uart_puts(" not found, try <help>\n");
  }
}

int bootloader_shell_help(char * string_buffer)
{
  UNUSED(string_buffer);
  for(int command_idx = 0; bootloader_shell_command_list[command_idx] != 0x0; ++command_idx)
  {
    uart_puts(bootloader_shell_command_list[command_idx]);
    uart_puts(": ");
    uart_puts(bootloader_shell_command_descriptions[command_idx]);
    uart_putc('\n');
  }
  return 0;
}

