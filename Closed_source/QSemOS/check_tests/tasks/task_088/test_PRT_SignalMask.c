#include "test_common.h"
#include "stubs/prt_signal.h"
#include "../task_077/func_under_test.c"
#include "func_under_test.c"

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    signalSet old = 0;

    // Case 1: get old only
    RUNNING_TASK->sigMask = sigMask(1);
    U32 ret = PRT_SignalMask(0, NULL, &old);
    print_result_int("get old ok", OS_OK, ret);
    print_result_hexu32("old mask", sigMask(1), old);

    // Case 2: block 2
    signalSet set = sigMask(2);
    ret = PRT_SignalMask(SIG_BLOCK, &set, NULL);
    print_result_int("block ok", OS_OK, ret);
    print_result_hexu32("mask now 1|2", sigMask(1) | sigMask(2), RUNNING_TASK->sigMask);

    // Case 3: unblock 1
    set = sigMask(1);
    ret = PRT_SignalMask(SIG_UNBLOCK, &set, NULL);
    print_result_int("unblock ok", OS_OK, ret);
    print_result_hexu32("mask now 2", sigMask(2), RUNNING_TASK->sigMask);

    // Case 4: setmask to 4
    set = sigMask(4);
    ret = PRT_SignalMask(SIG_SETMASK, &set, NULL);
    print_result_int("setmask ok", OS_OK, ret);
    print_result_hexu32("mask now 4", sigMask(4), RUNNING_TASK->sigMask);

    // Case 5: unblock triggers delivery
    add_pending_signal(RUNNING_TASK, 4); // masked, should not be delivered
    set = sigMask(4);
    ret = PRT_SignalMask(SIG_UNBLOCK, &set, NULL);
    print_result_int("unblock delivers ok", OS_OK, ret);
    print_result_int("pending cleared", 0, (RUNNING_TASK->sigPending & sigMask(4)) != 0);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
