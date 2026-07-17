#include "stub_prt_signal_env.h"
#include <stdio.h>

struct TagTskCb g_runningTask;

int g_total = 0;
int g_pass = 0;

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

void RESET_COUNTERS(void)
{
    g_total = 0;
    g_pass = 0;
}

void print_result_int(const char *name, int expected, int actual)
{
    printf("[CASE] %s\n", name);
    printf("  Expected: %d\n", expected);
    printf("  Actual  : %d\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

void print_result_ptr(const char *name, void *expected, void *actual)
{
    printf("[CASE] %s\n", name);
    printf("  Expected: %p\n", expected);
    printf("  Actual  : %p\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

void print_pass_rate(void)
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
}