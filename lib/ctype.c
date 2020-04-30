/* http://www.cplusplus.com/reference/cctype/isdigit/ */
int isdigit ( int c )
{
    if ( c <= '9' && c >= '0' )
        return 1;
    else
        return 0;
}

/* http://www.cplusplus.com/reference/cctype/isspace/ */
int isspace ( int c )
{
    if ( c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == 'f' || c == '\r' )
        return 1;
    return 0;
}

int isalpha ( int c )
{
    if ( ( c <= 'Z' && c >= 'A' )  || ( c <= 'z' && c >= 'a') )
        return 1;
    return 0;
}