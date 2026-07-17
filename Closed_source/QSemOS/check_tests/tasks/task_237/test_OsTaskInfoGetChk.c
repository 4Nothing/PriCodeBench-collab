#include "test_common.h"
#include "prt_task_internal.h"
#include "func_under_test.c"

// Globals
volatile U32 g_os_int_active = 0;
U32 g_this_core = 0;
TaskNameGetFunc g_taskNameGet = NULL;
TaskNameAddFunc g_taskNameAdd = NULL;
struct TagTskCb g_taskTbl[TEST_MAX_TASKS];
U32 g_tskBaseId = 0;
U32 g_tskMaxNum = TEST_MAX_TASKS - 1;

U32 OsTaskInfoGetChk(TskHandle taskPid, struct TskInfo *taskInfo);

int main(void)
{
    struct TskInfo dummy;

    // Case 1: null pointer
    ASSERT_EQ_U32(OS_ERRNO_TSK_PTR_NULL, OsTaskInfoGetChk(0, NULL));

    // Case 2: overflow id
    ASSERT_EQ_U32(OS_ERRNO_TSK_ID_INVALID, OsTaskInfoGetChk(TEST_MAX_TASKS, &dummy));

    // Case 3: max valid id
    ASSERT_EQ_U32(OS_OK, OsTaskInfoGetChk(TEST_MAX_TASKS - 1, &dummy));

    // Case 4: zero id
    ASSERT_EQ_U32(OS_OK, OsTaskInfoGetChk(0, &dummy));

    // Case 5: random valid id
    ASSERT_EQ_U32(OS_OK, OsTaskInfoGetChk(3, &dummy));

    print_pass_rate();
    return 0;
}
