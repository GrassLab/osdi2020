//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

void reverse(char str[], int length) { 
    int start = 0; 
    int end = length -1; 
    while (start < end) { 
		char tmp  = *(str+start);
		*(str+start) = *(str+end);
		*(str+end) = tmp;
        //swap(*(str+start), *(str+end)); 
        start++; 
        end--; 
    } 
}

char* itoa(unsigned long long int num, char* str, unsigned long long int base) { 
    unsigned long long int i = 0; 
    _Bool isNegative = 0; 

    if (num == 0) { 
        str[i] = '0';
		i = i + 1;
        str[i] = '\0'; 
        return str; 
    } 
    if (num < 0 && base == 10) { 
        isNegative = 1; 
        num = -num; 
    } 
    while (num != 0) { 
        unsigned long long int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    }  
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0';
    reverse(str, i); 
    return str; 
}


int strcmp(const char *X, const char *Y){
	while(*X)
	{
		// if characters differ or end of second string is reached
		if (*X != *Y)
			break;

		// move to next pair of characters
		X++;
		Y++;
	}

	// return the ASCII difference after converting char* to unsigned char*
	return *(const unsigned char*)X - *(const unsigned char*)Y;
}

int strlen(const char *str){
    int len;
	for(len = 0; str[len]; len++);
	return len;
}
