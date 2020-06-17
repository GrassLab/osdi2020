#ifndef __SCHEDULE_CONTEXT_H
#define __SCHEDULE_CONTEXT_H

struct __TaskStruct;
struct __UserTaskStruct;
struct __KernelContext;
struct __UserContext;

extern struct __TaskStruct *getCurrentTask(void);
extern struct __UserTaskStruct *getUserCurrentTask(void);

void switchContext(struct __KernelContext *next_task);
extern void switchCpuContext(struct __KernelContext *prev,
                             struct __KernelContext *next);

extern void initUserTaskandSwitch(struct __KernelContext *prev,
                                  struct __UserContext *next);

void switchUserToKernel(void);
extern void switchToEl1(struct __KernelContext *prev,
                        struct __UserContext *next);

void switchKernelToUser(void);
extern void switchToEl0(struct __UserContext *prev,
                        struct __KernelContext *next);

#endif
