#include "schedule/context.h"

void switchContext(struct __TaskStruct *next_task) {
    switchCpuContext(getCurrentTask(), next_task);
}
