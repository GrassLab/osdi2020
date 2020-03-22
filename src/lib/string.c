int strcmp(const char *X, const char *Y) {
	while(*X) {
		if (*X != *Y)
			break;
		X++;
		Y++;
	}

	return *(const unsigned char*)X - *(const unsigned char*)Y;
}

void swap(char* a, char* b) {
	char tmp = *a;
	*a = *b;
	*b = tmp;
}

void reverse(char str[], int length) {
  int start = 0;
  int end = length -1;

  while (start < end) {
	  swap(str+start, str+end);
	  start++;
	  end--;
  }
}

void itoa(int num, char* str) {
  int i = 0;
  if (num == 0) {
	  str[i++] = '0';
	  str[i] = '\0';
	  return;
  }

  while (num != 0) {
	  int rem = num % 10;
      str[i++] = (rem > 9)? (rem - 10) + 'a' : rem + '0';
      num = num / 10;
  }
  str[i] = '\0';
  reverse(str, i);
}
