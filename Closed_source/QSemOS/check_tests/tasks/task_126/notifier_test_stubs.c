#include "common/shim_prt_task_test.h"
#include <string.h>

struct TagTskCb g_runningTask;

void StubResetTask(void)
{
    memset(&g_runningTask, 0, sizeof(g_runningTask));
    g_runningTask.taskPid = 1;
}