/**
 * Minimal sprintf implementation
 **/
unsigned int vsprintf(char *dst, char *fmt, __builtin_va_list args)
{
    long int arg;
    int len, sign, i;
    char *p, *orig = dst, tmpstr[19];

    /* failsafes(pointer is null) */
    if(dst == (void*)0 || fmt == (void*)0) return 0;

    /* main loop */
    arg = 0;
    while(*fmt) {
        /* argument access */
        if(*fmt == '%') {
            fmt++;
            if(*fmt == '%') goto put;
        
            len = 0;
            /* size modifier */
            while(*fmt >= '0' && *fmt <= '9') {
                len *= 10;
                len += (*fmt - '0');
                fmt++;
            }
            /* skip long modifier */
            if(*fmt == 'l') fmt++;
            /* char */
            if(*fmt == 'c') {
                arg = __builtin_va_arg(args, int);
                *dst++ = (char)arg;
                fmt++;
                continue;
            }
            /* integer */
            else if(*fmt == 'd') {
                arg = __builtin_va_arg(args, int);
                /* handle sign */
                sign = 0;
                if((int)arg < 0) {
                    arg *= -1;
                    sign++;
                }
                /* handle large number */
                if(arg > 99999999999999999L) {
                    arg = 99999999999999999L;
                }
                /* convert int to string*/
                i = 18;
                tmpstr[i] = 0; // '\0'
                do {
                    tmpstr[--i] = '0' + (arg % 10);
                    arg /= 10;
                } while(arg != 0 && i > 0);
                if(sign) tmpstr[--i] = '-';
                /* handle padding(space only) */
                if(len > 0 && len < 18) {
                    while(i > 18 - len) {
                        tmpstr[--i] = ' ';
                    }
                }
                p = &tmpstr[i]; // string start addr
                goto copystring;
            }
            /* hex */
            else if(*fmt == 'x') {
                arg = __builtin_va_arg(args, long int);
                /* convert to string */
                i = 16;
                tmpstr[i] = 0; // '\0'
                do {
                    char n = arg & 0xf;
                    /* 0-9 -> offset 48(0x30), else offset 55*/
                    tmpstr[--i] = n + (n > 9 ? 0x37 : 0x30);
                    arg >>= 4;
                } while(arg != 0 && i > 0);
                /* handle padding(leading zero) */
                if(len > 0 && len <= 16) {
                    while(i > 16 - len) tmpstr[--i] = '0';
                }
                p = &tmpstr[i]; // string start addr
                goto copystring;
            }
            /* string */
            else if(*fmt == 's') {
                p = __builtin_va_arg(args, char*);
copystring:     if(p == (void*)0) p = "(null)";
                while(*p) {
                    *dst++ = *p++;
                }
            }
        }
        else {
put:        *dst++ = *fmt;
        }
        fmt++;
    }
    *dst = 0; // '\0'
    return dst - orig; // number of bytes written
}

/**
 * Variable length args
 **/
unsigned int sprintf(char *dst, char *fmt, ...)
{
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    return vsprintf(dst, fmt, args);
}
