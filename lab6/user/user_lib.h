#pragma once

void user_write(char * buf);
int user_fork();
void user_exit();
int user_read();
void user_mem_status();
int get_taskid();
int remain_page_num();

int strcmp(const char *s1, const char *s2);
void *memset(void *s, int c, int n);
void user_print(char *format, ...);
void user_println(char *format, ...);

extern void user_delay(unsigned long n);
