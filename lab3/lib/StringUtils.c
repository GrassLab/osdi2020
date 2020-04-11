void stripString(char *str) {
    while (*str != '\0') {
        if (*str == '\n') {
            *str = '\0';
            return;
        }
        ++str;
    }
}

int compareString(const char *s1, const char *s2) {
    unsigned char c1, c2;

    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0') {
            return c1 - c2;
        }
    } while (c1 == c2);

    return c1 - c2;
}

// only take 10-base integer string
unsigned int getIntegerFromString(const char *str) {
    unsigned int value = 0u;

    while (*str) {
        value = value * 10u + (*str - '0');
        ++str;
    }
    return value;
}
