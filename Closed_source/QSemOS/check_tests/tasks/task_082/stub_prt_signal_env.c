#include "stub_prt_signal_env.h"
#include <stdio.h>

struct TagTskCb g_runningTask;

void StubResetTask(void)
{
    memset(&g_runningTask, 0, sizeof(g_runningTask));
    g_runningTask.taskPid = 1;
    ListInit(&g_runningTask.sigInfoList);
    for (int i = 0; i < (int)PRT_SIGNAL_MAX; ++i)
    {
        g_runningTask.sigVectors[i] = NULL;
    }
}

void add_pending_signal(struct TagTskCb *task, int signum)
{
    task->sigPending |= sigMask(signum);
}
