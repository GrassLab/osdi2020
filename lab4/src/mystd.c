int strcmp(char *s1, char *s2){
    while(*s1 || *s2){
        if(*s1 != *s2)
            return 0;
        ++s1; ++s2;
    }
    return 1;
}

int atoi(char *str){
    int res = 0; 
    for (int i = 2; str[i] != '\0'; ++i) 
        res = res * 16 + str[i] - '0';  
    return res; 
}


int strlen(char *s){
	int size = 0;

	while(*s) {
		size++;
		s++;
	}

	return size;
}

void swap(char *xp, char *yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

void reverse(char str[], int length) 
{ 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap(str+start, str+end); 
        start++; 
        end--; 
    } 
} 

char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    int isNegative = 0; 
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) 
    { 
        isNegative = 1; 
        num = -num; 
    } 
  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
    if (base == 16){ 
        str[i++] = 'x';
        str[i++] = '0';
    } 
  
    str[i] = '\0'; // Append string terminator 
  
    // Reverse the string 
    reverse(str, i); 
  
    return str; 
} 

void strAppend(char *s1, char *s2){
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	for(int i=0; i<len2; i++){
		s1[len1 + i] = s2[i];
	}

	s1[len1 + len2] = '\0';

}
