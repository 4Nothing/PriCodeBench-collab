#include "test_support.h"
#include "func_under_test.c"

/* Include implementation so inline functions are visible */

int main(void)
{
    int pass = 0, total = 0;
    reset_task();

    /* Case 1: task context returns running PID */
    total++;
    g_os_hwi_active = 0;
    g_running_task->taskPid = 42;
    U32 act1 = OsGetSrcPid();
    pass += assert_eq_u32("OsGetSrcPid task ctx", 42U, act1);

    /* Case 2: HWI active returns HWI handle composed (0xFFFF) */
    total++;
    g_os_hwi_active = 1;
    U32 act2 = OsGetSrcPid();
    pass += assert_eq_u32("OsGetSrcPid HWI ctx", 0xFFFFU, act2);

    /* Case 3: back to task context with different PID */
    total++;
    g_os_hwi_active = 0;
    g_running_task->taskPid = 7;
    U32 act3 = OsGetSrcPid();
    pass += assert_eq_u32("OsGetSrcPid task ctx 2", 7U, act3);

    /* Case 4: HWI overrides task PID */
    total++;
    g_os_hwi_active = 1;
    g_running_task->taskPid = 1234;
    U32 act4 = OsGetSrcPid();
    pass += assert_eq_u32("OsGetSrcPid HWI overrides", 0xFFFFU, act4);

    /* Case 5: multiple toggles end in task ctx again */
    total++;
    g_os_hwi_active = 0;
    g_running_task->taskPid = 1;
    U32 act5 = OsGetSrcPid();
    pass += assert_eq_u32("OsGetSrcPid final task", 1U, act5);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
