#ifndef _STRING_INCLUDE_H_
#define _STRING_INCLUDE_H_

inline int strlen(const char *s)
{
	int __res = 0;
	while (*(s + __res++) != '\0')
		;
	return --__res;
}
inline int strcmp(const char *cs, const char *ct)
{
	if (strlen(cs) != strlen(ct))
		return -1;
	while (*cs) {
		if (*cs++ != *ct++)
			return -1;
	}
	return 0;
}
#endif