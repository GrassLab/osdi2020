#include "uart.h"
#include "mystd.h"
#include "exc.h"
#include "irq.h"
#include "task.h"
#include "timer.h"

void foo(){
    char out[128] = {0};
    char buf[32] = {0};
    int tmp = 5;

    strAppend(out, "Task ");

    ullToStr( (unsigned long long)get_taskId(), buf);
    strAppend(out, buf);
    strAppend(out, " after exec, tmp address 0x");

    ullToStr_hex((unsigned long long)&tmp, buf);
    strAppend(out, buf);
    strAppend(out, ", tmp value ");

    ullToStr((unsigned long long)tmp, buf);
    strAppend(out, buf);
    strAppend(out, "\n");

    uart_write(out);

    exit(0);
}

void test() {
    int cnt = 1;

    if (fork() == 0) {
        fork();
        sleep();
        fork();

        while(cnt < 10) {
            char out[128] = {0};
            char buf[32] = {0};
            strAppend(out, "Task id: ");

            int taskid = get_taskId();
            ullToStr((unsigned long long)taskid, buf);
            strAppend(out, buf);
            strAppend(out, ", cnt: ");

            ullToStr((unsigned long long)cnt, buf);
            strAppend(out, buf);
            strAppend(out, ", cnt address: 0x");

            ullToStr_hex((unsigned long long)&cnt, buf);
            strAppend(out, buf);
            strAppend(out, "\n");


            uart_write(out);

            sleep();
            ++cnt;
        }

        exit(0);
        uart_write("Should not be printed\n");
    } else {
        char out[128] = {0};
        char buf[32] = {0};

        strAppend(out, "Task ");

        ullToStr((unsigned long long)get_taskId(), buf);
        strAppend(out, buf);
        strAppend(out, " before exec, cnt address 0x");

        ullToStr_hex((unsigned long long)&cnt, buf);
        strAppend(out, buf);
        strAppend(out, ", cnt value ");

        ullToStr((unsigned long long)cnt, buf);
        strAppend(out, buf);
        strAppend(out, "\n");

        uart_write(out);

        exec(foo);
    }
}

void user_test(){
  do_exec(test);
}



void main()
{
    uart_init();

    core_timer_enable();
    init_Queue(&runQueue);

    privilege_task_create(idle);
    privilege_task_create(zombieReaper);

    privilege_task_create(user_test);

    go_to(&idle_pcb->context);
}
