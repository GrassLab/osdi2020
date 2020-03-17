#ifndef MY_STRING
#define MY_STRING

int strcmp(const char *x, const char *y);

char* strtok(char *x, const char *delim); // Work In Process

void reverse(char *x, char *y);

void itoa(long long x, char *res);

void unsign_itoa(unsigned long long x, char *res);

void ftoa(float x, char *i_res, char *f_res);

#endif
