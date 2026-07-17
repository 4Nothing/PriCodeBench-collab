#include "test_common.h"
#include "func_under_test.c"

static inline TimerHandle make_handle(U32 type, U32 id) { return (TimerHandle)((type << 28) | (id & 0x0FFFFFFFu)); }

static U32 ok_getOverrun(TimerHandle h, U32 *out)
{
    (void)h;
    *out = 42;
    return 0;
}

int main(void)
{
    U32 ret;
    TimerHandle h;
    U32 over = 0;

    memset(g_timerApi, 0, sizeof(g_timerApi));

    // Case 1: invalid handle type
    h = make_handle(TIMER_TYPE_INVALID, 1);
    ret = PRT_TimerGetOverrun(0, h, &over);
    print_result("Invalid handle type", OS_ERRNO_TIMER_HANDLE_INVALID, ret);

    // Case 2: missing getOverrun handler
    h = make_handle(TIMER_TYPE_SWTMR, 2);
    ret = PRT_TimerGetOverrun(0, h, &over);
    print_result("Missing getOverrun handler", OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED, ret);

    // Case 3: success SW, verify out value
    g_timerApi[TIMER_TYPE_SWTMR].getOverrun = ok_getOverrun;
    over = 0;
    ret = PRT_TimerGetOverrun(0, h, &over);
    print_result("Success SW (ret)", 0, ret);
    print_result("Success SW (out)", 42, over);

    // Case 4: success HW
    g_timerApi[TIMER_TYPE_HWTMR].getOverrun = ok_getOverrun;
    h = make_handle(TIMER_TYPE_HWTMR, 3);
    over = 0;
    ret = PRT_TimerGetOverrun(0, h, &over);
    print_result("Success HW (ret)", 0, ret);
    print_result("Success HW (out)", 42, over);

    // Case 5: invalid handle type 3
    h = make_handle(6u, 4); // 6 is >= TIMER_TYPE_INVALID
    ret = PRT_TimerGetOverrun(0, h, &over);
    print_result("Invalid handle type 3", OS_ERRNO_TIMER_HANDLE_INVALID, ret);

    return print_summary();
}
