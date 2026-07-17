#include <stdio.h>
#include <string.h>
#include "prt_task_internal.h"

volatile int g_hwi_lock_count = 0;

extern volatile U32 g_os_int_active;
extern U32 g_this_core;

bool OsTaskStackIsSave(struct TagTskCb *taskCB) {
    return taskCB->stackSaved != 0;
}

bool OsTaskSpPcInfoReady(struct TagTskCb *taskCB) {
    return ((((taskCB->taskStatus & OS_TSK_RUNNING) == 0) || (g_os_int_active) ||
        (taskCB->coreID != g_this_core)) && OsTaskStackIsSave(taskCB));
}

uintptr_t OsTskGetInstrAddr(uintptr_t stackPtr) {
    return stackPtr + 4;
}

uintptr_t OsGetSp(void) {
    return 0xDEADBEEF;
}

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
