/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "shell.h"
#include "device/uart.h"
#include "task/taskManager.h"
#include "task/schedule.h"
#include "task/switch.h"
#include "task/runnableTask.h"

#define INIT_TASK \
/*cpu_context*/ { { 0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	 0, 0} \

void main()
{
    // set up serial console
    uartInit();

    // for(int i = 0; i < 3; ++i) { // N should > 2
    //     createPrivilegeTask(&kernelTask, 0);
    // }

    createPrivilegeTask(&execTask, 0); 

    // Create a null init task
    struct task init_task = INIT_TASK;
    current = &init_task;

    // Enable core timer
    // asm volatile("svc #2");

    idleTask();

    // runShell();
}
