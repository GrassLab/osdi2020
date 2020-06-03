int atoi(char* input)
{
    int res;
    int i;
    int isNegative = 0;

    res = 0;
    i = 0;
    while (input[i] != '\0') {
        int current;

        if (i == 0 && input[i] == '-') {
            isNegative = 1;
            i++;
            continue;
        }
        current = input[i] - '0';
        res = res * 10 + current;
        i++;
    }

    if (isNegative)
        res *= -1;

    return res;
}

void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int hex2int(char* hex)
{
    int val = 0;
    while (*hex) {
        // get current character then increment
        int byte1 = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte1 >= '0' && byte1 <= '9')
            byte1 = byte1 - '0';
        else if (byte1 >= 'a' && byte1 <= 'f')
            byte1 = byte1 - 'a' + 10;
        else if (byte1 >= 'A' && byte1 <= 'F')
            byte1 = byte1 - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte1 & 0xF);
    }
    return val;
}

char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x /= 10;
    }

    if (i == 0) {
        str[i++] = '0';
    }

    // while(i<d) {
    // 	str[i++] = '0';
    // }
    reverse(str, i);

    // for(int j=0; j<i; j++) {
    // 	if(str[j] == '0') {
    // 		for(int)
    // 	}
    // }

    str[i] = '\0';
    return i;
}

int pow(int a, int b)
{
    int res = 1;
    for (int i = 0; i < b; i++) {
        res *= a;
    }

    return res;
}

void ftoa(float n, char* res, int afterpoint)
{
    int ipart = (int)n;
    float fpart = n - (float)ipart;

    int i = intToStr(ipart, res, 0);

    if (afterpoint != 0) {
        res[i] = '.';
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

int strcmp(char* a, char* b)
{
    while (*a != '\0') {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    if (*a == *b)
        return 1;
    return 0;
}
