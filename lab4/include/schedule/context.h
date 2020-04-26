#ifndef __SCHEDULE_CONTEXT_H
#define __SCHEDULE_CONTEXT_H

struct __TaskStruct;

extern struct __TaskStruct *getCurrentTask(void);
extern void switchCpuContext(struct __TaskStruct *prev,
                             struct __TaskStruct *next);
extern void switchContext(struct __TaskStruct *next_task);

#endif
