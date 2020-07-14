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

int strncpy(char *dest, const char *src,int n)
{
    int c = 0;
   
    // not a good design to skip space directly here, but I am such a lazy guy...... 
    while(src[c]!='\0')
    {
        dest[c] = src[c];
	c++;

	if(c>=n)
		break;
    }

    dest[c] = '\0';
    return c;
}

int strcpy_delim(char *dest, const char *src,int len,const char delim){

	int c = 0;
  	while (src[c] != '\0' && src[c] != delim && len>c){
    		dest[c]	= src[c];
		c++;
	}
  
  	dest[c] = '\0';
	return c;
}

int strtolower(char *str){
	int c=0;
  	
	while (str[c] != '\0' ){
    		str[c] = ((str[c] >= 'A' && str[c] <= 'Z') ? str[c] - 'A' + 'a':str[c]);
		c++;
	}

	return c;
}


int strlen ( const char * s )
{
    int i = 0;
    while ( 1 )
    {
        if ( *( s + i ) == '\0' )
            break;
        i++;
    }

    return i;
}

char *strcat (char *dest, const char *src)
{
  strcpy (dest + strlen (dest), src);
  return dest;
}
