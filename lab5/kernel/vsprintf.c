#include "string.h"
#include "uart.h"

#define ZEROPAD 1 /* pad with zero */
#define SIGN 2 /* unsigned/signed long */
#define PLUS 4 /* show plus */
#define SPACE 8 /* space if plus */
#define LEFT 16 /* left justified */
#define SPECIAL 32 /* 0x */
#define SMALL 64 /* use 'abcdef' instead of 'ABCDEF' */

#define is_digit(x) (x >= '0' && x <= '9')
unsigned long long int do_div(unsigned long long int n, int base)
{
	return n % base;
}

char *int_to_str(char *str, unsigned long num, int base, int size,
		 int precision, int type)
{
	const char *digits = "0123456789abcdef";
	char tmp[36]; // max lengh of int
	long i = 0;
	if (num == 0) {
		tmp[i++] = digits[0];
	} else {
		if (base == 16) { // should be 16 disable for now
			unsigned char n;
			int c;
			for (c = 0; c < 32; c += 4) {
				// get highest tetrad
				n = (num >> c) & 0xF;
				// 0-9 => '0'-'9', 10-15 => 'A'-'F'
				n += n > 9 ? 0x37 : 0x30;
				tmp[i++] = n;
			}
		} else {
			while (num != 0) {
				tmp[i++] = digits[do_div(num, base)];
				num = num / base;
			}
		}
	}
	if (i > precision)
		precision = i;
	if (num < 0) {
		*str++ = '-';
	}
	// if (type & SPECIAL) {
	// 	if (base == 8) {
	// 		*str++ = '0';
	// 	} else if (base == 16) {
	// 		*str++ = '0';
	// 		*str++ = digits[33];
	// 	}
	// }
	if (base == 16) { // for print address
		*str++ = '0';
		*str++ = 'x';
	}
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0) {
		*str++ = tmp[i];
	}
	*str = '\0';
	return str;
}

int getPrecision(int precision)
{
	int i = 1;
	while (precision-- > 0) {
		i *= 10;
	}
	return i;
}
char *float_to_str(char *str, float num, int size, int precision, int type)
{
	int base = 10;
	int integer = (int)num;
	precision = precision > 0 ? precision : 8;
	int point = (num - integer) * getPrecision(precision);
	str = int_to_str(str, num, base, size, 0, type);
	int len = strlen(str);
	str[len++] = '.';
	type |= ZEROPAD;
	str = int_to_str(str + len, point, base, 0, precision, type);
	return str;
}
int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

char *vsprintf(char *buf, const char *fmt, __builtin_va_list args)
{
	unsigned long long int num;
	float float_num;
	char *str;
	char *s;
	void *ptr;
	for (str = buf; *fmt; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
		int flags = 0;
	repeat:
		++fmt;
		switch (*fmt) {
		case '-':
			flags |= LEFT;
			goto repeat;
		case '+':
			flags |= PLUS;
			goto repeat;
		case ' ':
			flags |= SPACE;
			goto repeat;
		case '#':
			flags |= SPECIAL;
			goto repeat;
		case '0':
			flags |= ZEROPAD;
			goto repeat;
		}
		int field_width = -1;
		if (is_digit(*fmt)) {
			field_width = skip_atoi(&fmt);
		}
		int precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			if (precision < 0)
				precision = 0;
		}
		switch (*fmt) {
		case 'c': // get char
			*str++ = (char)__builtin_va_arg(args, int);
			break;
		case 's': // get string
			s = __builtin_va_arg(args, char *);
			int len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			while (*s) {
				*str++ = *s++;
			}
			break;
		case 'p': // print pointer in hex
			if (field_width == -1) {
				field_width = 16;
				flags |= ZEROPAD;
			}
			ptr = __builtin_va_arg(args, void *);
			str = int_to_str(str, (unsigned long)ptr, 16,
					 field_width, precision, flags);
			break;
		case 'x':
			num = __builtin_va_arg(args, unsigned long long int);
			str = int_to_str(str, num, 16, field_width, precision,
					 flags);
			break;
		case 'd': // get integer
			num = __builtin_va_arg(args, unsigned long long int);
			str = int_to_str(str, num, 10, field_width, precision,
					 flags);
			break;
		case 'f': // get float
			float_num = __builtin_va_arg(args, double);
			str = float_to_str(str, float_num, field_width,
					   precision, flags);
			break;
		default:
			if (*fmt != '%') {
				*str++ = '%';
			}
			if (*fmt) {
				*str++ = *fmt;
			} else {
				--fmt;
				break;
			}
		}
	}
	*str = '\0';
	return buf;
}