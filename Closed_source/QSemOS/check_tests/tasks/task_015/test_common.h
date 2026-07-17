#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
#include "../include/prt_timer_external.h"

typedef U32 (*TimerCreateFunc)(struct TimerCreatePara *, TimerHandle *);
struct TagTimerApi {
    TimerCreateFunc createTimer;
    TimerDeleteFunc deleteTimer;
    TimerQueryFunc queryTimer;
    TimerStartFunc startTimer;
    TimerStopFunc stopTimer;
    TimerSetIntervalFunc setInterval;
    TimerRestartFunc restartTimer;
    TimerGetOverrunFunc getOverrun;
};
#define TIMER_API_NUM 2
static struct TagTimerApi g_timerApi[TIMER_API_NUM];

static int g_total = 0;
static int g_pass = 0;

static void print_result(const char *case_name, U32 expected, U32 actual)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%08X\n", expected);
    printf("  Actual  : 0x%08X\n", actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    g_total++;
    g_pass += ok;
}

static int print_summary()
{
    double rate = g_total ? (100.0 * g_pass / g_total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, g_pass, g_total);
    return (g_pass == g_total) ? 0 : 1;
}

#endif // TEST_COMMON_H
