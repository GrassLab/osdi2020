#ifndef _UTILS_H
#define _UTILS_H

#define CHECK(ret) \
    if (!ret) { \
        return -1; \
    }

extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);
extern int get_el(void);

#ifndef __ASSEMBLER__
int strcmp(const char*, const char*);
void strcpy(char*, const char*);
#endif

#endif /*_UTILS_H */
