#include "uart.h"
#include "str.h"
int my_strcmp(const char *X, const char *Y, int size)
{
    int count=0;

	while(*X && (count < size))
	{
		// if characters differ or end of second string is reached
		if (*X != *Y)
			break;

		// move to next pair of characters
		X++;
		Y++;
        count++;
	}
    if(count == size)
    {
        return 0;
    }
	// return the ASCII difference after converting char* to unsigned char*
	return *(const unsigned char*)X - *(const unsigned char*)Y;
}

int my_strlen(const char *str)
{
    int len;
    for (len = 0; str[len]; len++);
    return len;
}

void my_strset(char *str, char rep, int size)
{
    for(int i=0;i<size;i++)
        str[i]=rep;
}

void str_reverse(char str[], int length)
{
    int start = 0; 
    int end = length -1; 
    char tmp;
    while (start < end) 
    { 
        tmp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = tmp;
        start++; 
        end--; 
    } 
}

void my_itoa(int num, char *str, int base)
{
    int i = 0;
    short neg=0;
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return; 
    } 
    if (num < 0 && base==10)
    {
        neg=1;
        num=-num;
    }
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
    if (neg) 
        str[i++] = '-'; 
    str[i] = '\0';
    str_reverse(str, i); 

}

void my_strcat(char *dst, char *src)
{
    char *ptr = dst + my_strlen(dst);
    while(*src != '\0')
        *ptr++ = *src++;
    *ptr='\0';
}

long my_atoi(const char* S)
{
	long num = 0;

	int i = 0;
	
	// run till we have reached end of the string or
	// current character is non-numeric
	while (S[i] && (S[i] >= '0' && S[i] <= '9'))
	{
		num = num * 10 + (S[i] - '0');
		i++;
	}

	return num;
}

int my_strcpy(char *s1, const char *s2, int size)
{
    char *tmp = s1;
    int count=0;
	if ((s1 == 0) || (s2 == 0))
	{
		return 0;
	}

	for (; *s2 != '\0'; tmp++, s2++)
	{
        if(count >= size) break;
        //uart_send(*s2);
		*tmp = *s2;
        count++;
	}
    *tmp=0;

	return 1;

}

int byte_to_int(char *buf, int size)
{
    int sum=0;
    for(int i=(size-1);i>=0;i--)
    {
        sum+=((int)buf[i] << 8*i);
    }
    return sum;
}

long long sp_byte_to_int(char *buf)
{
    int sum = byte_to_int(buf, 4);
    int check = sum & 0xFFFFFFF;
    if(check == 0xFFFFFF7) return BAD_CLU;
    if(check>=0xFFFFFF8 && check<=0xFFFFFFF) return FILE_EOF;
    if(check>=0xFFFFFF0 && check<=0xFFFFFF6) return RESV_CLU;
    if(check>=0x0000002 && check<=0xFFFFFEF) return check;
    if(check==0x0000001) return RESV_CLU;
    if(check==0x0000000) return NO_CLU;
}

void mystr_strip(char *buf)
{
    int len = my_strlen(buf);
    for(int i=len-1;i>=0;i--)
    {
        if(buf[i] == 0x20)
        {
            buf[i]=0;
        }
        else
        {
            break;
        }
    }

}