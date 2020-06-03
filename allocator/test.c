#include <stdio.h>
#include <stdlib.h>
#include "buddy.h"
#include "obj_allocator.h"

/*
 ** Testcase for buddy system
 */
void testcase1()
{
    buddy_init();
    buddy_view();
    buddy_alloc(0x100);
    buddy_view();
    buddy_alloc(0x400);
    buddy_view();
    buddy_alloc(0x1000);
    buddy_view();
}

void testcase2()
{
    buddy_init();
    buddy_view();
    buddy_alloc(0x101);
    buddy_view();
}

void testcase3()
{
    buddy_init();
    buddy_view();
    buddy_alloc(0x1000);
    buddy_view();
    buddy_alloc(0x100);
    buddy_view();
}

void testcase4()
{
    buddy_init();
    buddy_view();
    unsigned long addr1 = buddy_alloc(0x100);
    buddy_view();
    printf("addr: 0x%lx\n", addr1);
    buddy_free(addr1);
    buddy_view();
}
/*
 ** Testcase for Fixed-size object allocator
 */
void testcase5()
{
    init_allocator();
    unsigned long token1 = registration(0x100);
    unsigned long addr1 = fixed_alloc(token1);
    unsigned long addr2 = fixed_alloc(token1);
    unsigned long addr3 = fixed_alloc(token1);
    // printf("token = 0x%lx\n", token);
    allocator_view();
    // printf("addr1 = 0x%lx\n", addr1);
    fixed_free(addr1);
    fixed_free(addr2);
    fixed_free(addr3);
    allocator_view();
}

void testcase6()
{
    buddy_init();
    init_allocator();
    varied_alloc(0x4000);
    varied_alloc(0x4000);
    buddy_view();
    allocator_view();

}

/*
 ** Varied-size object allocator
 */
int main()
{
    // printf("This is demo of buddy system\n");
    // testcase1();
    // testcase2();
    // testcase3();
    // testcase4();

    // printf("This is demo of Fixed-size object allocator\n");
    // testcase5();
    
    printf("This is demo of Varied-size object allocator\n");
    testcase6();
    

    return 0;
}
