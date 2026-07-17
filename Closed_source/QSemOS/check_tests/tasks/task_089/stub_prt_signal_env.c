#include "auto_stub.h"

struct TagTskCb g_runningTask;
TagSwTmrSortLinkCtx g_tmrSortLink = {0};

void StubResetTask(void)
{
    memset(&g_runningTask, 0, sizeof(g_runningTask));
    g_runningTask.taskPid = 1;
    INIT_LIST_OBJECT(&g_runningTask.sigInfoList);
    for (int i = 0; i < (int)PRT_SIGNAL_MAX; ++i)
    {
        g_runningTask.sigVectors[i] = NULL;
    }
}
