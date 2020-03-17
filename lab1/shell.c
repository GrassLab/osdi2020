#include "shell.h"
#include "miniuart.h"
#include "meta_macro.h"
#include "string_util.h"
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
  int time_frequency = get_cntfrq_el0();
  int time_counter = get_cntpct_el0();
  float current_time = (float)time_counter / (float)time_frequency;
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

