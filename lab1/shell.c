#include "shell.h"
#include "miniuart.h"
#include "meta_macro.h"
#include "string_util.h"

const char * shell_command_list[] = {
  "hello",
  "help",
  0x0
};

const char * shell_command_descriptions[] = {
  "Print Hello World!",
  "Help",
  0x0
};

int (*shell_command_function_ptr[])(const char *) = {
  shell_hello,
  shell_help,
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
  string_strip_newline(string_buffer);



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

int shell_hello(const char * string_buffer)
{
  UNUSED(string_buffer);
  miniuart_puts("Hello World!\n");
  return 0;
}

int shell_help(const char * string_buffer)
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

