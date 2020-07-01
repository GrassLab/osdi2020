int strcmp(const char *str1, const char *str2){
    int i = 0;
    while(str1[i] != '\0' || str2[i] != '\0'){   
        if(str1[i] != str2[i]){
           return -1;
        }
        i++;
    }
    return 0;
}

void strcpy(char *dest, const char *src){
    int i = 0;
    while( *(src + i) != '\0' ){
        dest[i] = *(src + i);
        i++;
    }
    dest[i] = *(src + i);
    return;
}

int strtolower(char *str){
    int c=0;

    while (str[c] != '\0' ){
            str[c] = ((str[c] >= 'A' && str[c] <= 'Z') ? str[c] - 'A' + 'a':str[c]);
        c++;
    }

    return c;
}

int strcpy_delim(char *dest, const char *src,int len,const char delim){

    int c = 0;
    while (src[c] != '\0' && src[c] != delim && len > c){
        dest[c] = src[c];
        c++;
    }

    dest[c] = '\0';
    return c;
}

int strncpy(char *dest, const char *src,int n)
{
    int c = 0;

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
