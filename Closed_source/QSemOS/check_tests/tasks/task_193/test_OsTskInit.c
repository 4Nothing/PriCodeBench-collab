#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset_env(void)
{
    UNI_FLAG = 0;
    stub_OsTskSMPInit_ret = OS_OK;
    g_taskScanHook = NULL;
}

static void case_success_sets_flag_and_hook()
{
    reset_env();
    U32 r = OsTskInit();
    total++;
    passed += run_check("return OS_OK", OS_OK, r);
    total++;
    passed += run_check("flag OS_FLG_TSK_SWHK set", OS_FLG_TSK_SWHK, UNI_FLAG & OS_FLG_TSK_SWHK);
    total++;
    passed += run_check("taskScanHook set", 1, g_taskScanHook ? 1 : 0);
}

static void case_smpinit_fail_bubbles_up()
{
    reset_env();
    stub_OsTskSMPInit_ret = 0x55u;
    U32 r = OsTskInit();
    total++;
    passed += run_check("return fail code", 0x55, r);
    total++;
    passed += run_check("no flag set on fail", 0, (int)(UNI_FLAG & OS_FLG_TSK_SWHK));
}

static void case_multiple_calls_idempotent_flag()
{
    reset_env();
    (void)OsTskInit();
    U32 first = UNI_FLAG;
    (void)OsTskInit();
    total++;
    passed += run_check("flag unchanged after second call", first, UNI_FLAG);
}

static void case_hook_not_null_after_init()
{
    reset_env();
    (void)OsTskInit();
    total++;
    passed += run_check("hook non-null", 1, g_taskScanHook != NULL);
}

static void case_hook_callable()
{
    reset_env();
    (void)OsTskInit();
    // Just ensure calling doesn't crash
    if (g_taskScanHook)
        g_taskScanHook();
    total++;
    passed += run_check("hook callable", 1, 1);
}

int main()
{
    case_success_sets_flag_and_hook();
    case_smpinit_fail_bubbles_up();
    case_multiple_calls_idempotent_flag();
    case_hook_not_null_after_init();
    case_hook_callable();
    double passRate = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", passRate);
    return (passed == total) ? 0 : 1;
}
