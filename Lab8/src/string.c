int get_length(char str[]){
	int i = 0;
	while(str[i]!='\0'){
		i++;
	}
	return i;
}

void reverse_string(char* str){ //just for some special use
	int start,end;
	int length = get_length(str);
	start = 0;
	char temp;
 
        for(end=length-1;end>=0;end--){
		if(start<end){
                         temp = str[end];
                         str[end] = str[start];
                         str[start] = temp;
                         start++;
         	}
         }
}

int itos(unsigned long long num,char* str,int base){
          unsigned long long sum = num;
          int i = 0;
          int digit;
  
          do
          {
                  digit = sum % base;
                  if (digit < 0xA)
                          str[i++] = '0' + digit;
                  else
                          str[i++] = 'A' + digit - 0xA;
                  sum /= base;
          }while (sum);
	
          str[i] = '\0';
	  reverse_string(str);
          return 0;
}

int strcmp (const char *p1, const char *p2){
    const unsigned char *s1 = (const unsigned char *) p1;
    const unsigned char *s2 = (const unsigned char *) p2;
    unsigned char c1, c2;
    do
      {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        if (c1 == '\0')
          return c1 - c2;
      }
    while (c1 == c2);
    return c1 - c2;
 }


int strcpy(char *dest, const char *src)
{
	int c = 0;
  	while (src[c] != '\0'){
    		dest[c]	= src[c];
		c++;
	}
  
  	dest[c] = '\0';
	return c;
}
