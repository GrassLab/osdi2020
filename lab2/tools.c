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

int hexChar2int(char ch){
	int num;
	if(ch>='0' && ch<='9'){
		num = ch - 0x30;
	}else{
		switch(ch){
			case 'A': case 'a': num=10; break;
			case 'B': case 'b': num=11; break;
			case 'C': case 'c': num=12; break;
			case 'D': case 'd': num=13; break;
			case 'E': case 'e': num=14; break;
			case 'F': case 'f': num=15; break;
			default: num=0;
		}
	}
	return num;
}

void timestamp(int *integer_part, int *decimal_point_part){
	unsigned long long int tmp_CNTPCT_EL0;
	unsigned long long int tmp_CNTFRQ_EL0;
	int i1, i2;
	float f1, f2;
	// get the time
	asm volatile("mrs %0, CNTPCT_EL0" : "=r" (tmp_CNTPCT_EL0));
	asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (tmp_CNTFRQ_EL0));
	// divide operation
	f1 = (float) ( (float)tmp_CNTPCT_EL0 / (float)tmp_CNTFRQ_EL0 );
	i1 = f1;
	f2 = f1 - i1;	
	while(1) {
		i2 = f2;
		if((float)i2 == f2) break;
		f2 = f2 * 10;
	}
	*integer_part = i1;
	*decimal_point_part = i2;

}

