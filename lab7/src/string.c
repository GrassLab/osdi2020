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
    return;
}

