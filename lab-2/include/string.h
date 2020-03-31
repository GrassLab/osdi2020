#ifndef STRING_H
#define STRING_H

void strInvert(char *str);
int strlen(char *str);
int strEqual(char *str1, char *str2);
void intToStr(int num, char *str);
void doubleToStr(double num, char *str);
void hexToStr(unsigned int num, char *str);
int strToInt(char *str);
int strToNum(char *str, int base);

#endif