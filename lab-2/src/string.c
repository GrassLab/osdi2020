#include "string.h"
#include "utility.h"
#include "uart.h"

void strInvert(char *str) {
    int size = strlen(str);
    char buffer[size];
    for (int i = 0; i < size; i++) {
        buffer[i] = str[size-i-1];
    }
    for (int i = 0; i < size; i++) {
        str[i] = buffer[i];
    }
}

int strlen(char *str){
    int len = 0;
    while(str[len] != '\0') len ++;
    return len;
}

int strEqual(char *str1, char *str2)
{
    int str1len = strlen(str1);
    int str2len = strlen(str2);
    int length = (str1len > str2len) ? str1len : str2len;
    for (int i = 0; i < length; i++) {
        if (str1[i] != str2[i]) return 0;
    }
    return 1;
}

void intToStr(int num, char *str) {
    int index = 0;
    while(num > 0) {
        str[index++] = (num % 10) + 48;
        num /= 10;
    }
    if (num == 0 && index ==0) str[index++] = '0';
    str[index] = '\0';
    strInvert(str);
}

void doubleToStr(double num, char *str) {
    int intPart = (int)num;
    char intPartStr[128] = {0};
    intToStr(intPart, intPartStr);
    int intPartStrSize = strlen(intPartStr);
    for (int i = 0; i < intPartStrSize; i++) {
        str[i] = intPartStr[i];
    }
    str[intPartStrSize] = '.';
    
    char floatPartStr[128] = {0};
    double floatPart = num - (double) intPart;

    floatPart *= 100000000;
    while((int)floatPart % 10 == 0 ) floatPart /= 10;
    intToStr((int)floatPart, floatPartStr);
    int floatPartStrSize = strlen(floatPartStr);

    for (int i = intPartStrSize + 1; i < intPartStrSize + floatPartStrSize + 1; i++) {
        str[i] = floatPartStr[i - intPartStrSize - 1];
    }
    str[intPartStrSize + 1 + floatPartStrSize] = '\0';
}


void hexToStr(unsigned int num, char *str) {
    unsigned int n;
    for (int c = 28; c >= 0; c -= 4) {
        n = (num >> c) & 0xF;
        n += n > 9 ? 0x37: 0x30;
        (str)[c/4] = n;
    }
    for (int i = 7; i >= 0; i -= 1) {
        if((str)[i] == '0') (str)[i] = '\0';
        else break;
    }
    strInvert(str);
}

void hexToStrNoTruncat(unsigned int num, char *str) {
    unsigned int n;
    for (int c = 28; c >= 0; c -= 4) {
        n = (num >> c) & 0xF;
        n += n > 9 ? 0x37: 0x30;
        (str)[c/4] = n;
    }
    str[8] = '\0';
    strInvert(str);
}

int strToInt(char *str) 
{
    int num = 0;
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        num += (str[i] - 48) * pow(10, len-i-1);
    }
    return num;
}

int strToNum(char *str, int base)
{
    int num = 0;
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
        num += (str[i] - 48) * pow(base, len-i-1);
    }
    return num;
}