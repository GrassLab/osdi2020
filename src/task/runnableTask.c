#include "type.h"
#include "device/uart.h"
#include "task/taskManager.h"
#include "task/schedule.h"
#include "task/sysCall.h"
#include "memory/memManager.h"
#include "memory/buddy.h"
#include "memory/memPool.h"
#include "filesystem/filesystem.h"

extern uint64_t _binary_user_img_start;
extern uint64_t _binary_user_img_end;

void delay(uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        asm volatile("nop");
    }
}

// User task

void userTask()
{
    // asm volatile("svc #2"); // Enable timer

    uartPuts("user task\n");
    exit(0);
}

void foo()
{
    uint32_t tmp = 5;
    uartPuts("Task ");
    uartInt(getTaskId());
    uartPuts(" after exec, tmp address ");
    uartHex((uint64_t)&tmp);
    uartPuts(", tmp value ");
    uartInt(tmp);
    uartPuts("\n");
    exit(0);
}

void forkTask()
{
    // asm volatile("svc #2"); // Enable timer

    uint32_t cnt = 1;

    if (fork() == 0)
    {
        fork();

        delay(5000000);

        fork();

        while (cnt < 10)
        {
            uartPuts("Task id: ");
            uartInt(getTaskId());
            uartPuts(", cnt: ");
            uartInt(cnt);
            uartPuts("\n");

            delay(5000000);

            ++cnt;
        }

        exit(0);
        uartPuts("Should not be printed\n");
    }
    else
    {
        uartPuts("Task ");
        uartInt(getTaskId());
        uartPuts(" before exec, cnt address ");
        uartHex((uint64_t)&cnt);
        uartPuts(", cnt value ");
        uartInt(cnt);
        uartPuts("\n");
        exec(&foo);
    }
}

// Kernel task

void execTask()
{
    uint64_t begin = (uint64_t)&_binary_user_img_start;
    uint64_t end = (uint64_t)&_binary_user_img_end;

    doExec(begin, end - begin, 0x80000);
}

void idleTask()
{
    while (1)
    {
        schedule();

        for (uint32_t i = 0; i < 1000000; ++i)
        {
            asm volatile("nop");
        }
    }
}

void kernelTask()
{
    while (1)
    {
        // if (current->re_schedule == true)
        // {
            uartPuts("Task id: ");
            uartInt(current->task_id);
            uartPuts("  issue reschedule\n");

            for (uint32_t i = 0; i < 1000000; ++i)
            {
                asm volatile("nop");
            }

            current->re_schedule = false;
            schedule();
        // }
    }
}

void _printAddr(uint64_t addr)
{
    uartPuts("get free memory address: ");
    uartHex(addr);
    uartPuts("\n");
}

void memTestTask()
{
    uint64_t addr0 = allocFreePage(0);
    _printAddr(addr0);
    freePage(addr0);

    int32_t token = getFreePool(32);
    if (token != -1)
    {
        uint64_t addr1 = allocSlot(token);
        _printAddr(addr1);

        uint64_t addr2 = allocSlot(token);
        _printAddr(addr2);

        freeSlot(token, addr1);
        freeSlot(token, addr2);
        freePool(token);
    }

    uint64_t addr3 = allocDynamic(8);
    _printAddr(addr3);

    uint64_t addr4 = allocDynamic(5000);
    _printAddr(addr4); 

    freeDynamic(addr3);
    freeDynamic(addr4);

    busyloop();
}

void vfsTask()
{
    char buf[512];
    struct file* a = vfsOpen("hello", O_CREAT);
    // struct file* b = vfsOpen("world", O_CREAT);
    // struct file* c = vfsOpen("hello", 0);
    // struct file* d = vfsOpen("test/hello", O_CREAT);
    // struct file* e = vfsOpen("test/hello", 0);

    vfsWrite(a, "Hello ", 6);
    // vfsWrite(b, "World!", 6);

    // vfsClose(a);
    // vfsClose(b);
    // vfsClose(c);
    // vfsClose(d);
    // vfsClose(e);

    a = vfsOpen("hello", 0);
    // a = vfsOpen("world", 0);
    int sz;
    sz = vfsRead(a, buf, 100);
    // sz += vfsRead(a, buf + sz, 100);
    // buf[sz] = '\0';

    uartPuts("read: ");
    uartPuts(buf);
    uartPuts("\n");

    // uartPuts("read size: ");
    // uartInt(sz);
    // uartPuts("\n");

    // mkdir("test2");
    // d = vfsOpen("test2/hello", O_CREAT);
    // vfsClose(d);
    
    // lsdir("test2");
    // lsdir("/");

    busyloop();
}