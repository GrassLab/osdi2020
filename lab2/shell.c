#include "shell.h"
#include "miniuart.h"
#include "meta_macro.h"
#include "string_util.h"
#include "mailbox.h"
#include "pm.h"

const char * shell_command_list[] = {
  "hello",
  "help",
  "timestamp",
  "reboot",
  0x0
};

const char * shell_command_descriptions[] = {
  "Print Hello World!",
  "Help",
  "Get current timestamp",
  "Reboot rpi3",
  0x0
};

int (*shell_command_function_ptr[])(char *) = {
  shell_hello,
  shell_help,
  shell_timestamp,
  shell_reboot,
  0x0
};


char pikachu0[] =
  ""ANSI_BG_GREEN".......***.................................................."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".......****................................................."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".......***."ANSI_BG_YELLOW" "ANSI_BG_GREEN"................................................"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"......****."ANSI_BG_YELLOW"   "ANSI_BG_GREEN".............................................."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".........*."ANSI_BG_YELLOW"     "ANSI_BG_GREEN"............................................"ANSI_RESET"\n"
  ""ANSI_BG_GREEN".........*."ANSI_BG_YELLOW"       "ANSI_BG_GREEN"............................***********.."ANSI_BG_YELLOW" "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..........*"ANSI_BG_YELLOW"        "ANSI_BG_GREEN"...........................********.."ANSI_BG_YELLOW"    "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..........*."ANSI_BG_YELLOW"         "ANSI_BG_GREEN".........................******.."ANSI_BG_YELLOW"      "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............"ANSI_BG_YELLOW"          "ANSI_BG_GREEN".........................***."ANSI_BG_YELLOW"         "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............"ANSI_BG_YELLOW"           "ANSI_BG_GREEN".........................."ANSI_BG_YELLOW"           "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............."ANSI_BG_YELLOW"            "ANSI_BG_GREEN"......................"ANSI_BG_YELLOW"             "ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............*."ANSI_BG_YELLOW"            "ANSI_BG_GREEN"..................."ANSI_BG_YELLOW"              "ANSI_BG_GREEN"."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............*."ANSI_BG_YELLOW"            "ANSI_BG_GREEN".....           ."ANSI_BG_YELLOW"                "ANSI_BG_GREEN"*"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"............**."ANSI_BG_YELLOW"                                           "ANSI_BG_GREEN".*"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"...........****."ANSI_BG_YELLOW"                                         "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"...........*****."ANSI_BG_YELLOW"                                        "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"........*********."ANSI_BG_YELLOW"  .                                   "ANSI_BG_GREEN"...."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..****************..."ANSI_BG_YELLOW"                                 "ANSI_BG_GREEN"......"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..*****************."ANSI_BG_YELLOW"                              "ANSI_BG_GREEN".. ......."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".*****************."ANSI_BG_YELLOW"                                "ANSI_BG_GREEN"........."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".*****************."ANSI_BG_YELLOW"                                 "ANSI_BG_GREEN"........"ANSI_RESET"\n"
  ""ANSI_BG_GREEN".****************."ANSI_BG_YELLOW"      "ANSI_RESET"..."ANSI_BG_YELLOW"               "ANSI_RESET"..."ANSI_BG_YELLOW"       "ANSI_BG_GREEN"........"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"****************.."ANSI_BG_YELLOW"     "ANSI_RESET"."ANSI_BG_WHITE" "ANSI_RESET".*."ANSI_BG_YELLOW"             "ANSI_RESET"."ANSI_BG_WHITE" "ANSI_RESET".*."ANSI_BG_YELLOW"       "ANSI_BG_GREEN"......."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"***************.*."ANSI_BG_YELLOW"     "ANSI_RESET". .*."ANSI_BG_YELLOW"             "ANSI_RESET". .*."ANSI_BG_YELLOW"       "ANSI_BG_GREEN"......."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"**************..."ANSI_BG_YELLOW"      "ANSI_RESET".***."ANSI_BG_YELLOW"             "ANSI_RESET".***."ANSI_BG_YELLOW"       "ANSI_BG_GREEN".*....."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"**************..."ANSI_BG_YELLOW"       "ANSI_RESET"***"ANSI_BG_YELLOW"               "ANSI_RESET"***"ANSI_BG_YELLOW"         "ANSI_BG_GREEN"...***"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"**************.*."ANSI_BG_YELLOW"                                     "ANSI_BG_GREEN".*****"ANSI_RESET"\n";
char pikachu1[] =
  ""ANSI_BG_GREEN"**************.."ANSI_BG_YELLOW"               ...                    "ANSI_BG_GREEN".*****"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"**************.."ANSI_BG_YELLOW" "ANSI_BG_RED"..."ANSI_BG_YELLOW"           "ANSI_RESET"...."ANSI_BG_YELLOW"            "ANSI_BG_RED".."ANSI_BG_YELLOW"      "ANSI_BG_GREEN"*****"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"*************..."ANSI_BG_RED"....."ANSI_BG_YELLOW"                         "ANSI_BG_RED"....."ANSI_BG_YELLOW"    "ANSI_BG_GREEN".****"ANSI_RESET"\n"
  ""ANSI_BG_GREEN"************...."ANSI_BG_RED"......"ANSI_BG_YELLOW"                       "ANSI_BG_RED"......."ANSI_BG_YELLOW"   "ANSI_BG_GREEN"....."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"**********......"ANSI_BG_RED"......"ANSI_BG_YELLOW"                       "ANSI_BG_RED"......."ANSI_BG_YELLOW"   "ANSI_BG_GREEN"....."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"*.............. "ANSI_BG_RED"....."ANSI_BG_YELLOW"          "ANSI_BG_RED"......"ANSI_BG_YELLOW"        "ANSI_BG_RED"......."ANSI_BG_YELLOW"    "ANSI_BG_GREEN"...."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................"ANSI_BG_YELLOW" "ANSI_BG_RED"..."ANSI_BG_YELLOW"          "ANSI_BG_RED"........"ANSI_BG_YELLOW"       "ANSI_BG_RED"......."ANSI_BG_YELLOW"    "ANSI_BG_GREEN"...."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................"ANSI_BG_YELLOW"              "ANSI_BG_RED"........"ANSI_BG_YELLOW"        "ANSI_BG_RED"....."ANSI_BG_YELLOW"     "ANSI_BG_GREEN"...."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................"ANSI_BG_YELLOW"              "ANSI_BG_RED"........"ANSI_BG_YELLOW"                  "ANSI_BG_GREEN"...."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................"ANSI_BG_YELLOW"               "ANSI_BG_RED"......."ANSI_BG_YELLOW"                   "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................."ANSI_BG_YELLOW"               "ANSI_BG_RED"....."ANSI_BG_YELLOW"                    "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"................. "ANSI_BG_YELLOW"                                       "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".................."ANSI_BG_YELLOW"                                       "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".................."ANSI_BG_YELLOW"                                       "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..................."ANSI_BG_YELLOW"                                      "ANSI_BG_GREEN"..."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..................."ANSI_BG_YELLOW"                                       "ANSI_BG_GREEN".."ANSI_RESET"\n"
  ""ANSI_BG_GREEN"..................."ANSI_BG_YELLOW"                                       "ANSI_BG_GREEN".."ANSI_RESET"\n"
  ""ANSI_BG_GREEN".................*"ANSI_BG_YELLOW"                                        "ANSI_BG_GREEN".."ANSI_RESET"\n";

void shell(void)
{
  char string_buffer[0x1000];

  miniuart_init();

	/* Show boot message */

  miniuart_puts(pikachu0);
  miniuart_puts(pikachu1);
  shell_show_board_revision(string_buffer);
  shell_show_vc_memory(string_buffer);

  while(1)
  {
    miniuart_puts(ANSI_RED "# " ANSI_RESET);
    miniuart_gets(string_buffer, '\n', 0x1000 - 1);
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
    if(string_cmp(string_buffer, shell_command_list[command_idx]))
    {
      (*shell_command_function_ptr[command_idx])(string_buffer);
      break;
    }
  }
  if(shell_command_list[command_idx] == 0x0)
  {
    miniuart_puts("Err: command ");
    miniuart_puts(string_buffer);
    miniuart_puts(" not found, try <help>\n");
  }
}

int shell_hello(char * string_buffer)
{
  UNUSED(string_buffer);
  miniuart_puts("Hello World!\n");
  return 0;
}

int shell_help(char * string_buffer)
{
  UNUSED(string_buffer);
  for(int command_idx = 0; shell_command_list[command_idx] != 0x0; ++command_idx)
  {
    miniuart_puts(shell_command_list[command_idx]);
    miniuart_puts(": ");
    miniuart_puts(shell_command_descriptions[command_idx]);
    miniuart_putc('\n');
  }
  return 0;
}

int shell_timestamp(char * string_buffer)
{
  unsigned long long time_frequency = get_cntfrq_el0();
  unsigned long long time_counter = get_cntpct_el0();
  float current_time = (float)((double)time_counter / (double)time_frequency);
  string_float_to_char(string_buffer, current_time);
  miniuart_putc('[');
  miniuart_puts(string_buffer);
  miniuart_puts("]\n");
  return 0;
}

int shell_reboot(char * string_buffer)
{
  UNUSED(string_buffer);

  miniuart_puts("Reboot...");
  /* Full reset */
  *PM_RSTC = PM_PASSWORD | 0x20;
  /* Reset in 10 tick */
  *PM_WDOG = PM_PASSWORD | 10;
  /* Stuck */
  while(1);
  return 0;
}

void shell_show_board_revision(char * string_buffer)
{
  miniuart_puts("Board revision: ");
  string_longlong_to_char(string_buffer, mailbox_get_board_revision());
  miniuart_puts(string_buffer);
  miniuart_putc('\n');
  return;
}

void shell_show_vc_memory(char * string_buffer)
{
  uint32_t vc_memory_base;
  uint32_t vc_memory_size;

  if(!mailbox_get_vc_memory())
  {
    miniuart_puts("Unable to get vc memory\n");
    return;
  }

  vc_memory_base = __mailbox_buffer[5];
  vc_memory_size = __mailbox_buffer[6];

  miniuart_puts("VC core base address: ");
  string_longlong_to_char(string_buffer, vc_memory_base);
  miniuart_puts(string_buffer);
  miniuart_puts(" size: ");
  string_longlong_to_char(string_buffer, vc_memory_size);
  miniuart_puts(string_buffer);
  miniuart_putc('\n');
}
