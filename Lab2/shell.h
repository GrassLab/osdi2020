#ifndef SHELL
#define SHELL
#define COMC 3

void wait_command();
void command(char *s);

void timestamp(const char *args);
void reboot(const char *args);
void loadimg(const char *args);
void reset();

int is_num_string(const char *s);
int string_num(const char *s, int base);
void num_string(long long num, char *s, int base);

int strcmp(const char *s1, const char *s2);
void strcpy(const char *s1, char *s2);
int length(const char *s);

struct hash{
    void (*function)(const char *);
    char *str;
//    struct hash *next;
};

#endif