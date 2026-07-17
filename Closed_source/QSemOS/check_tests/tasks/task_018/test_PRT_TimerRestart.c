#include "test_common.h"
#include "func_under_test.c"

static inline TimerHandle make_handle(U32 type, U32 id) { return (TimerHandle)((type << 28) | (id & 0x0FFFFFFFu)); }

static U32 ok_restart(TimerHandle h)
{
    (void)h;
    return 0;
}

int main(void)
{
    U32 ret;
    TimerHandle h;

    memset(g_timerApi, 0, sizeof(g_timerApi));

    // Case 1: invalid handle type
    h = make_handle(TIMER_TYPE_INVALID, 1);
    ret = PRT_TimerRestart(0, h);
    print_result("Invalid handle type", OS_ERRNO_TIMER_HANDLE_INVALID, ret);

    // Case 2: missing restart handler
    h = make_handle(TIMER_TYPE_SWTMR, 2);
    ret = PRT_TimerRestart(0, h);
    print_result("Missing restart handler", OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED, ret);

    // Case 3: success SW
    g_timerApi[TIMER_TYPE_SWTMR].restartTimer = ok_restart;
    ret = PRT_TimerRestart(0, h);
    print_result("Success SW", 0, ret);

    // Case 4: success HW
    g_timerApi[TIMER_TYPE_HWTMR].restartTimer = ok_restart;
    h = make_handle(TIMER_TYPE_HWTMR, 3);
    ret = PRT_TimerRestart(0, h);
    print_result("Success HW", 0, ret);

    // Case 5: another invalid handle type pattern
    h = make_handle(6u, 4); // 6 is >= TIMER_TYPE_INVALID
    ret = PRT_TimerRestart(0, h);
    print_result("Invalid handle type 3", OS_ERRNO_TIMER_HANDLE_INVALID, ret);

    return print_summary();
}
