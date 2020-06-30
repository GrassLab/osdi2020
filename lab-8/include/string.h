#ifndef STRING_H
#define STRING_H

int strcpy(char *dest, const char *src);
void strInvert(char *str);
int strlen(char *str);
int str_equal(char *str1, char *str2);
void strConcat(char *result, char *s1, char *s2);
void intToStr(int num, char *str);
void doubleToStr(double num, char *str);
void hexToStr(unsigned int num, char *str);
int strToInt(char *str);
int strToNum(char *str, int base);

#endif