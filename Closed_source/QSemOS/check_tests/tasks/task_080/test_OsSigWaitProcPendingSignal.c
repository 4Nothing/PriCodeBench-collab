#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    signalSet set = sigMask(3) | sigMask(5);
    signalInfo out;

    // Case 1: no pending -> returns OK, wait mask cleared to 0
    RUNNING_TASK->sigWaitMask = set;
    U32 ret = OsSigWaitProcPendingSignal(RUNNING_TASK, &set, &out);
    print_result_int("no pending -> OK", OS_OK, ret);
    print_result_hexu32("wait mask cleared", 0, RUNNING_TASK->sigWaitMask);

    // Prepare pending: 3 and 7
    add_pending_signal(RUNNING_TASK, 3);
    add_pending_signal(RUNNING_TASK, 7);

    // Case 2: mask includes 3 -> consumes 3
    RUNNING_TASK->sigWaitMask = set;
    memset(&out, 0, sizeof(out));
    ret = OsSigWaitProcPendingSignal(RUNNING_TASK, &set, &out);
    print_result_int("consume 3 -> OK", OS_OK, ret);
    print_result_int("out.si_signo == 3", 3, out.si_signo);
    print_result_int("sigPending cleared for 3", 0, (RUNNING_TASK->sigPending & sigMask(3)) != 0);

    // Case 3: remaining 7 not in set -> not touched
    print_result_int("7 still pending", 1, (RUNNING_TASK->sigPending & sigMask(7)) != 0);

    // Case 4: set matches 7 -> consumes 7
    signalSet set2 = sigMask(7);
    memset(&out, 0, sizeof(out));
    ret = OsSigWaitProcPendingSignal(RUNNING_TASK, &set2, &out);
    print_result_int("consume 7 -> OK", OS_OK, ret);
    print_result_int("out.si_signo == 7", 7, out.si_signo);

    // Case 5: list empty now
    print_result_int("no pending left", 0, RUNNING_TASK->sigPending != 0);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
