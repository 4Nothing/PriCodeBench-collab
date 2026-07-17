#include "stub_prt_signal_env.h"
#include <stdio.h>

/* Global running task for tests - defined here, declared extern in auto_stub.h */
struct TagTskCb g_runningTask;

/* Test counters */
int g_total = 0;
int g_pass = 0;

/* Reset counters */
void RESET_COUNTERS(void)
{
    g_total = 0;
    g_pass = 0;
}

/* Stub reset task */
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

/* Add pending signal - properly adds to sigInfoList */
void add_pending_signal(struct TagTskCb *task, int signum)
{
    sigInfoNode *infoNode = (sigInfoNode *)PRT_MemAlloc((U32)OS_MID_SIGNAL, OS_MEM_DEFAULT_FSC_PT, sizeof(sigInfoNode));
    if (infoNode == NULL) {
        return;
    }
    infoNode->siginfo.si_signo = signum;
    infoNode->siginfo.si_code = 0;
    ListTailAdd(&infoNode->siglist, &task->sigInfoList);
    task->sigPending |= sigMask(signum);
}

/* Print result int */
void print_result_int(const char *case_name, int expected, int actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: %d\n", expected);
    printf("  Actual  : %d\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

/* Print result ptr */
void print_result_ptr(const char *case_name, void *expected, void *actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: %p\n", expected);
    printf("  Actual  : %p\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

/* Print pass rate */
void print_pass_rate(void)
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
}