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

inline char *strncpy(char *s1, const char *s2, int n)
{
	int i;
	for (i = 0; i <= n; i++) {
		s1[i] = s2[i];
	}
	s1[i] = '\0';

	return s1;
}

inline char *strcpy(char *s1, const char *s2)
{
	return strncpy(s1, s2, strlen(s2));
}

#endif