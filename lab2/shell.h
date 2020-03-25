#ifndef __SHELL_H__
#define __SHELL_H__

void shell(void);
void _shell_parser();
int shell_hello(char * string_buffer);
int shell_help(char * string_buffer);
int shell_timestamp(char * string_buffer);
int shell_reboot(char * string_buffer);
int shell_show_board_revision(char * string_buffer);
int shell_show_vc_memory(char * string_buffer);
int shell_show_text_location(char * string_buffer);

extern unsigned long long get_cntfrq_el0(void);
extern unsigned long long get_cntpct_el0(void);
extern char __executable_start;
extern char __etext;

#endif

