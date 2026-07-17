#include "test_common.h"
#include "prt_task_internal.h"
#include <string.h>
#include "func_under_test.c"

// Globals
volatile U32 g_os_int_active = 0;
U32 g_this_core = 0;
TaskNameGetFunc g_taskNameGet = NULL;
TaskNameAddFunc g_taskNameAdd = NULL;
struct TagTskCb g_taskTbl[TEST_MAX_TASKS];

uintptr_t OsTaskSpInfoGet(struct TagTskCb *taskCb);
bool OsTaskSpPcInfoReady(struct TagTskCb *taskCB);

int main(void)
{
    struct TagTskCb t = {0};
    memset(&t, 0, sizeof(t));
    U32 stack[64];
    t.stackPointer = &stack[10];
    t.stackSaved = 1;
    t.coreID = 0;
    g_this_core = 0;

    // Case 1: ready -> get from tcb
    t.taskStatus = 0;
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], OsTaskSpInfoGet(&t));

    // Case 2: running, same core, not interrupt -> fallback to OsGetSp
    t.taskStatus = OS_TSK_RUNNING;
    g_os_int_active = 0;
    ASSERT_EQ_UPTR((uintptr_t)0xDEADBEEF, OsTaskSpInfoGet(&t));

    // Case 3: in interrupt -> from tcb
    g_os_int_active = 1;
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], OsTaskSpInfoGet(&t));

    // Case 4: different core -> from tcb
    g_os_int_active = 0;
    t.coreID = 1;
    g_this_core = 0;
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], OsTaskSpInfoGet(&t));

    // Case 5: not saved -> fallback regardless of others
    t.stackSaved = 0;
    ASSERT_EQ_UPTR((uintptr_t)0xDEADBEEF, OsTaskSpInfoGet(&t));

    print_pass_rate();
    return 0;
}
