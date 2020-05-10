#include "user_lib.h"

void main()
{
	char str[4];	
	str[0] = 'I';str[1] = 'M';str[2]='\n';str[3]='\0';
        uart_write(str,sizeof(str));

	char str1[]="I'm user";
        uart_write(str1,sizeof(str1));
	while(1);
}
