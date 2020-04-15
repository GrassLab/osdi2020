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
