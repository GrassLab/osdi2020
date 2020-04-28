#include "signal.h"
#include "task.h"
#include "printf.h"
#include "debug.h"

void (*task_signal_handler[TASK_NUM][SIGNAL_NUM])() = {0};

void signal(int num, void (*func)())
{
    int taskid = get_taskid();
    set_signal_handler(taskid, num, func);
}

void set_signal_handler(int taskid, int num, void (*func)())
{
    task_signal_handler[taskid][num] = func;
}

void default_handler()
{
}

void signal_raise(int taskid, int signal)
{
    task(taskid)->signal_source |= (1 << signal);
}

void signal_run(int taskid, int signal)
{
    // context_switch(task(get_current()), task(taskid));
    printf("ttttt");
    task_signal_handler[taskid][signal]();
    printf("ttttt");
}

void default_kill_handler()
{
    exit_process(get_current());
}

void signal_init()
{
    for (int i = 0; i < TASK_NUM; i++)
    {
        for (int j = 0; j < SIGNAL_NUM; j++)
        {
            if (j == SIGKILL)
            {

                task_signal_handler[i][j] = default_kill_handler;
            }
            else
            {
                task_signal_handler[i][j] = default_handler;
            }
        }
    }
}

void signal_router()
{
    int task_id = get_current();
    unsigned long ss = task(task_id)->signal_source;
    DEBUG_LOG_SIGNAL(("SIGNAL[%d][%d]", task_id, ss));
    if (ss != 0)
    {
        for (int i = 0; i < SIGNAL_NUM; i++)
        {
            if ((((1 << i) & ss) >> i) == 1)
            {
                DEBUG_LOG_SIGNAL(("[%d]", i));
                task_signal_handler[task_id][i]();
                task(task_id)->signal_source &= ~(1 << i);
                break;
            }
        }
    }
}