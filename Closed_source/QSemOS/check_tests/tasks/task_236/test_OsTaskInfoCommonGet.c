#include "test_common.h"
#include "prt_task_internal.h"
#include <string.h>
#include "func_under_test.c"

volatile U32 g_os_int_active = 0;
U32 g_this_core = 0;
TaskNameGetFunc g_taskNameGet = NULL;
TaskNameAddFunc g_taskNameAdd = NULL;
struct TagTskCb g_taskTbl[TEST_MAX_TASKS];

void OsTaskInfoCommonGet(struct TskInfo *taskInfo, struct TagTskCb *taskCb, TskHandle taskPid);

static char *mockNames[TEST_MAX_TASKS];
static void name_get(TskHandle id, char **name) { *name = mockNames[id]; }

int main(void)
{
    struct TagTskCb t = {0};
    struct TskInfo info;
    memset(&info, 0xAA, sizeof(info));

    // Prepare mock
    g_taskNameGet = name_get;
    mockNames[1] = "TaskOne";

    // Fill tcb
    t.taskStatus = 0x12;
    t.priority = 7;
    t.stackSize = 1024;
    t.topOfStack = (void *)0x1000;
    t.coreID = 2;
    t.taskEntry = (void *)0x12345678;
    t.taskPid = 1;

    // Case 1: copy fields & name
    OsTaskInfoCommonGet(&info, &t, 1);
    ASSERT_EQ_U32(0x12u, info.taskStatus);
    ASSERT_EQ_U32(7u, info.taskPrio);
    ASSERT_EQ_UPTR((uintptr_t)0x1000, info.topOfStack);
    ASSERT_EQ_U32(1024u, info.stackSize);
    ASSERT_EQ_U32(2u, info.core);
    ASSERT_EQ_UPTR((uintptr_t)0x12345678, info.entry);
    ASSERT_EQ_UPTR((uintptr_t)&t, (uintptr_t)info.tcbAddr);
    ASSERT_EQ_STR("TaskOne", info.name);

    // Case 2: long name truncated and null-terminated
    char longname[64];
    memset(longname, 'A', sizeof(longname));
    longname[63] = '\0';
    mockNames[2] = longname;
    t.taskPid = 2;
    OsTaskInfoCommonGet(&info, &t, 2);
    ASSERT_EQ_U32(OS_TSK_NAME_LEN - 1, (U32)strlen(info.name));
    ASSERT_EQ_U32('A', (U32)info.name[0]);

    // Case 3: g_taskNameGet = NULL -> name unchanged
    g_taskNameGet = NULL;
    strcpy(info.name, "Keep");
    OsTaskInfoCommonGet(&info, &t, 2);
    ASSERT_EQ_STR("Keep", info.name);

    // Case 4: verify different priority/core copy
    t.priority = 5;
    t.coreID = 0;
    OsTaskInfoCommonGet(&info, &t, 2);
    ASSERT_EQ_U32(5u, info.taskPrio);
    ASSERT_EQ_U32(0u, info.core);

    // Case 5: check tcbAddr again
    ASSERT_EQ_UPTR((uintptr_t)&t, (uintptr_t)info.tcbAddr);

    print_pass_rate();
    return 0;
}
