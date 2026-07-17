#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_env()
{
    UNI_FLAG = 0;
    stub_OsIdleTskSMPCreate_ret = OS_OK;
    g_primaryCoreId = 0;
    g_numOfCores = 1;
    stub_hw_thread_id = 0;
    stub_wakeup_secondary_called = 0;
    stub_first_time_switch_called = 0;
}

static void case_idle_create_fail()
{
    reset_env();
    stub_OsIdleTskSMPCreate_ret = 0x77u;
    U32 r = OsActivate();
    total++;
    passed += run_check("propagate fail", 0x77, r);
    total++;
    passed += run_check("no bgd active flag", 0, (int)(UNI_FLAG & OS_FLG_BGD_ACTIVE));
}

static void case_success_sets_flag_and_switch()
{
    reset_env();
    U32 r = OsActivate();
    total++;
    passed += run_check("return active_failed sentinel", OS_ERRNO_TSK_ACTIVE_FAILED, r);
    total++;
    passed += run_check("bgd active flag set", OS_FLG_BGD_ACTIVE, UNI_FLAG & OS_FLG_BGD_ACTIVE);
    total++;
    passed += run_check("first time switch called", 1, stub_first_time_switch_called == 1);
}

static void case_multi_core_wakeup_called()
{
    reset_env();
    g_numOfCores = 4;
    g_primaryCoreId = 0;
    stub_hw_thread_id = 0;
    (void)OsActivate();
    total++;
    passed += run_check("wakeup secondary called", 1, stub_wakeup_secondary_called == 1);
}

static void case_last_core_no_wakeup()
{
    reset_env();
    g_numOfCores = 4;
    g_primaryCoreId = 0;
    stub_hw_thread_id = 3; // last core
    (void)OsActivate();
    total++;
    passed += run_check("no wakeup on last core", 1, stub_wakeup_secondary_called == 0);
}

static void case_flag_persists()
{
    reset_env();
    (void)OsActivate();
    total++;
    passed += run_check("bgd flag set", OS_FLG_BGD_ACTIVE, UNI_FLAG & OS_FLG_BGD_ACTIVE);
    (void)OsActivate();
    total++;
    passed += run_check("bgd flag still set", OS_FLG_BGD_ACTIVE, UNI_FLAG & OS_FLG_BGD_ACTIVE);
}

int main()
{
    case_idle_create_fail();
    case_success_sets_flag_and_switch();
    case_multi_core_wakeup_called();
    case_last_core_no_wakeup();
    case_flag_persists();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}
