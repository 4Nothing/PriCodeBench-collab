#include "stub_prt_signal_env.h"

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

void add_pending_signal(struct TagTskCb *task, int signo)
{
    if (task == NULL || signo < 0 || signo >= (int)PRT_SIGNAL_MAX) return;
    
    // Set pending bit
    task->sigPending |= sigMask(signo);
    
    // Allocate and add sigInfoNode to list
    sigInfoNode *node = (sigInfoNode *)malloc(sizeof(sigInfoNode));
    if (node) {
        memset(&node->siginfo, 0, sizeof(node->siginfo));
        node->siginfo.si_signo = signo;
        ListInit(&node->siglist);
        ListTailAdd(&node->siglist, &task->sigInfoList);
    }
}
