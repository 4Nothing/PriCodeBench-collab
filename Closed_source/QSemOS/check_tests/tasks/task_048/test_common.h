#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"


static int g_total = 0;
static int g_pass = 0;

/* List operations */
static inline void ListInit(void *list)
{
    struct TagListObject *l = (struct TagListObject *)list;
    l->prev = l;
    l->next = l;
}



/* Task control block helpers */
static inline void init_tcb(void *tcb, int priority)
{
    struct TagTskCb *tsk = (struct TagTskCb *)tcb;
    tsk->taskPid = (U32)priority;
    tsk->priority = (U32)priority;
    tsk->taskPend = NULL;
    tsk->taskStatus = 0;
}

static inline int ptr_eq(const void *a, const void *b)
{
    return a == b;
}

static void print_case_result(int case_no, unsigned expected, unsigned actual)
{
    printf("[Case %d] Expected: %u, Actual: %u\n", case_no, expected, actual);
    printf("  Result  : %s\n\n", (expected == actual) ? "PASS" : "FAIL");
}

static void print_pass_rate(int pass, int total)
{
    double rate = (total == 0) ? 0.0 : (100.0 * (double)pass / (double)total);
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

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
