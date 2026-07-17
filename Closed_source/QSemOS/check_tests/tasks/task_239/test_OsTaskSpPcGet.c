#include "test_common.h"
#include "prt_task_internal.h"
#include <string.h>
#include "func_under_test.c"

volatile U32 g_os_int_active = 0;
U32 g_this_core = 0;
TaskNameGetFunc g_taskNameGet = NULL;
TaskNameAddFunc g_taskNameAdd = NULL;
struct TagTskCb g_taskTbl[TEST_MAX_TASKS];

void OsTaskSpPcGet(struct TagTskCb *taskCb, struct TskInfo *taskInfo);
bool OsTaskSpPcInfoReady(struct TagTskCb *taskCB);

int main(void)
{
    struct TagTskCb t = {0};
    struct TskInfo info = {0};
    U32 stack[64];

    memset(&t, 0, sizeof(t));
    t.stackPointer = &stack[10];
    t.stackSaved = 1;

    // Case 1: ready -> set sp,pc
    t.taskStatus = 0;
    g_os_int_active = 0;
    t.coreID = 0;
    g_this_core = 1; // different core
    OsTaskSpPcGet(&t, &info);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], info.sp);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10] + 4u, info.pc);

    // Case 2: not ready -> no change
    info.sp = 0;
    info.pc = 0;
    t.taskStatus = OS_TSK_RUNNING;
    g_os_int_active = 0;
    t.coreID = 0;
    g_this_core = 0;
    OsTaskSpPcGet(&t, &info);
    ASSERT_EQ_UPTR(0, info.sp);
    ASSERT_EQ_UPTR(0, info.pc);

    // Case 3: in interrupt -> ready
    t.taskStatus = OS_TSK_RUNNING;
    g_os_int_active = 1;
    OsTaskSpPcGet(&t, &info);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], info.sp);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10] + 4u, info.pc);

    // Case 4: not saved -> not set
    info.sp = 123;
    info.pc = 456;
    t.stackSaved = 0;
    g_os_int_active = 0;
    t.coreID = 1;
    g_this_core = 0;
    OsTaskSpPcGet(&t, &info);
    ASSERT_EQ_UPTR(123, info.sp);
    ASSERT_EQ_UPTR(456, info.pc);

    // Case 5: not running (ready) -> set
    t.stackSaved = 1;
    t.taskStatus = 0;
    g_os_int_active = 0;
    t.coreID = 0;
    g_this_core = 1;
    info.sp = 0;
    info.pc = 0;
    OsTaskSpPcGet(&t, &info);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10], info.sp);
    ASSERT_EQ_UPTR((uintptr_t)&stack[10] + 4u, info.pc);

    print_pass_rate();
    return 0;
}
