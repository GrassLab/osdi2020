void
reverse(char* str, int len)
{
	int i=0, j=len-1, temp;
	while(i<j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

int intToStr(int x, char str[], int d)
{
	int i=0;
	while(x) {
		str[i++] = (x%10) + '0';
		x /= 10;
	}

	while(i<d) {
		str[i++] = '0';
	}
	reverse(str, i);
	str[i] = '\0';
	return i;
}

int pow(int a, int b)
{
	int res = 1;
	for(int i=0; i<b; i++) {
		res *= a;
	}
	
	return res;
}

void
ftoa(float n, char* res, int afterpoint)
{
	int ipart = (int)n;
	float fpart = n-(float)ipart;

	int i = intToStr(ipart, res, 0);

	if(afterpoint != 0) {
		res[i] = '.';
		fpart = fpart * pow(10, afterpoint);

		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}
	
int
strcmp(char *a, char*b)
{
	while(*a != '\0') {
		if(*a != *b) return 0;
		a++;
		b++;
	}
	if(*a == *b) return 1;
	return 0;
}
