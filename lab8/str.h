#define FILE_EOF -1
#define BAD_CLU -2
#define OCC_CLU -3
#define NO_CLU -4
#define RESV_CLU -5
int my_strcmp(const char *a, const char *b, int size);
int my_strlen(const char *str);
void my_strset(char *str, char rep, int size);
void my_itoa(int num, char *str, int base);
void str_reverse(char str[], int length);
void my_strcat(char *dst, char *src);
long my_atoi(const char* S);
int my_strcpy(char *s1, const char *s2, int size);
int byte_to_int(char *buf, int size);
long long sp_byte_to_int(char *buf);
void mystr_strip(char *buf);