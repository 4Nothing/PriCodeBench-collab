#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_env()
{
    g_tskMaxNum = 0;
    UNI_FLAG = 0;
    stub_TaskSelf_ret = OS_OK;
    stub_TaskSelf_value = 42;
    stub_TaskGetInfo_called = 0;
    stub_TaskGetInfo_last_id = 0;
}

static void case_no_tasks()
{
    reset_env();
    TskHandle tid = 0;
    struct TskInfo info = {0};
    OsTskInfoGet(&tid, &info);
    total++;
    passed += run_check("no call when g_tskMaxNum==0", 0, stub_TaskGetInfo_called);
}

static void case_flag_not_active()
{
    reset_env();
    g_tskMaxNum = 10;
    UNI_FLAG = 0;
    TskHandle tid = 0;
    struct TskInfo info = {0};
    OsTskInfoGet(&tid, &info);
    total++;
    passed += run_check("no call when bgd inactive", 0, stub_TaskGetInfo_called);
}

static void case_self_fail()
{
    reset_env();
    g_tskMaxNum = 10;
    UNI_FLAG = OS_FLG_BGD_ACTIVE;
    stub_TaskSelf_ret = 0x9u;
    TskHandle tid = 0;
    struct TskInfo info = {0};
    OsTskInfoGet(&tid, &info);
    total++;
    passed += run_check("no getinfo when self fail", 0, stub_TaskGetInfo_called);
}

static void case_success_calls_getinfo()
{
    reset_env();
    g_tskMaxNum = 3;
    UNI_FLAG = OS_FLG_BGD_ACTIVE;
    stub_TaskSelf_ret = OS_OK;
    stub_TaskSelf_value = 77;
    TskHandle tid = 0;
    struct TskInfo info = {0};
    OsTskInfoGet(&tid, &info);
    total++;
    passed += run_check("getinfo called once", 1, stub_TaskGetInfo_called);
    total++;
    passed += run_check("threadId written", 77, tid);
    total++;
    passed += run_check("last id matches", 77, stub_TaskGetInfo_last_id);
}

static void case_multiple_invocations()
{
    reset_env();
    g_tskMaxNum = 2;
    UNI_FLAG = OS_FLG_BGD_ACTIVE;
    stub_TaskSelf_value = 5;
    TskHandle tid = 0;
    struct TskInfo info = {0};
    OsTskInfoGet(&tid, &info);
    OsTskInfoGet(&tid, &info);
    total++;
    passed += run_check("getinfo called twice", 2, stub_TaskGetInfo_called);
}

int main()
{
    case_no_tasks();
    case_flag_not_active();
    case_self_fail();
    case_success_calls_getinfo();
    case_multiple_invocations();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}
