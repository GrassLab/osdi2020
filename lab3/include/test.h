#ifndef __TEST_H__
#define __TEST_H__

#define ASSERT(expression) \
    do{ if(!(expression)) println("ASSERTION " #expression " FAILED. GET: ", expression); \
        else println("ASSERTION " #expression " PASSED."); \
    }while(0)

int bss_all_be();
int *stack_pointer();

#endif
