#include "test_common.h"
#include "func_under_test.c"

int main(void)
{
    /* Case 1: stop ok */
    g_stub_timer_delete_ret = OS_OK;
    set_core_id(0);
    g_primaryCoreId = 0;
    U32 ret = OsPerfTimedStop();
    print_case("stop ok", OS_OK, ret == OS_OK);

    /* Case 2: stop fails -> expect OS_ERROR */
    g_stub_timer_delete_ret = 0xD00DU;
    ret = OsPerfTimedStop();
    print_case("stop fails", OS_ERROR, ret == OS_ERROR);

    /* Case 3: SMP non-primary core should early return OS_OK (behavior guarded by macro in real build) */
    set_core_id(1);
    g_primaryCoreId = 0;
    g_stub_timer_delete_ret = OS_OK;
    ret = OsPerfTimedStop();
    print_case("non-primary core", OS_OK, ret == OS_OK);

    /* Case 4: multiple consecutive stops ok */
    set_core_id(0);
    g_stub_timer_delete_ret = OS_OK;
    ret = OsPerfTimedStop();
    print_case("stop again ok", OS_OK, ret == OS_OK);

    /* Case 5: ensure uses group 0 by contract (cannot check easily, but no crash) */
    ret = OsPerfTimedStop();
    print_case("no crash", OS_OK, ret == OS_OK);

    print_rate();
    return 0;
}
