#include <stdio.h>
#include <string.h>
#include "prt_task_internal.h"
#include "test_common.h"

extern struct TagTskCb g_taskTbl[];

void print_case_result(const char *name, int expected, int actual, int *passCnt, int *caseIdx)
{
    printf("[CASE %d] %s\n", *caseIdx, name);
    printf("  Expect: %d\n", expected);
    printf("  Actual: %d\n", actual);
    if (expected == actual)
    {
        (*passCnt)++;
        printf("  Result: PASS\n\n");
    }
    else
    {
        printf("  Result: FAIL\n\n");
    }
    (*caseIdx)++;
}
