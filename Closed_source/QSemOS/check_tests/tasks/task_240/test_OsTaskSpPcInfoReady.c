#include "test_common.h"
#include "prt_task_internal.h"
#include <string.h>
#include "func_under_test.c"

// Globals required by prt_task_info.c
volatile U32 g_os_int_active = 0;
U32 g_this_core = 0;
TaskNameGetFn g_taskNameGet = NULL;
TaskNameAddFn g_taskNameAdd = NULL;
struct TagTskCb g_taskTbl[TEST_MAX_TASKS];

// Under test declarations
bool OsTaskSpPcInfoReady(struct TagTskCb *taskCB);

int main(void)
{
    struct TagTskCb t = {0};
    memset(&t, 0, sizeof(t));
    t.stackSaved = 1; // stack saved
    t.coreID = 0;
    g_this_core = 0;

    // Case 1: not running, stack saved -> true
    t.taskStatus = 0; // not running
    ASSERT_EQ_BOOL(true, OsTaskSpPcInfoReady(&t));

    // Case 2: running, not in interrupt, same core, stack saved -> false
    t.taskStatus = OS_TSK_RUNNING;
    g_os_int_active = 0;
    t.coreID = 0;
    g_this_core = 0;
    ASSERT_EQ_BOOL(false, OsTaskSpPcInfoReady(&t));

    // Case 3: running but in interrupt -> true
    g_os_int_active = 1;
    ASSERT_EQ_BOOL(true, OsTaskSpPcInfoReady(&t));

    // Case 4: running, not interrupt, different core -> true
    g_os_int_active = 0;
    t.coreID = 1;
    g_this_core = 0;
    ASSERT_EQ_BOOL(true, OsTaskSpPcInfoReady(&t));

    // Case 5: conditions true but stack not saved -> false
    t.stackSaved = 0;
    ASSERT_EQ_BOOL(false, OsTaskSpPcInfoReady(&t));

    print_pass_rate();
    return 0;
}
