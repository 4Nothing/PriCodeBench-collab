#include "test_common.h"
#include "stubs/prt_signal.h"
#include "../task_077/func_under_test.c"
#include "../task_082/func_under_test.c"
#include "../task_084/func_under_test.c"
#include "func_under_test.c"

static int g_cnt = 0;
static void hcount(int s)
{
    (void)s;
    ++g_cnt;
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    // Case 1: pending not masked -> handler runs and returns
    g_cnt = 0;
    RUNNING_TASK->sigVectors[2] = hcount;
    add_pending_signal(RUNNING_TASK, 2);
    signalSet mask = sigMask(1); // 2 not masked
    U32 ret = PRT_SigSuspend(&mask);
    print_result_int("immediate deliver ok", OS_OK, ret);
    print_result_int("handler ran", 1, g_cnt);

    // Case 2: all masked -> will wait; our stub returns OK without real wait
    mask = ~0u;
    ret = PRT_SigSuspend(&mask);
    print_result_int("all masked wait ok", OS_OK, ret);

    // Case 3: mask blocks previously pending 4; add pending 4 then unblock via suspend
    RUNNING_TASK->sigVectors[4] = hcount;
    add_pending_signal(RUNNING_TASK, 4);
    mask = ~sigMask(4);
    ret = PRT_SigSuspend(&mask);
    print_result_int("unblock in suspend ok", OS_OK, ret);
    print_result_int("4 delivered", 1, (RUNNING_TASK->sigPending & sigMask(4)) == 0);

    // Case 4: idle task id check
    RUNNING_TASK->taskPid = IDLE_TASK_ID;
    mask = ~0u;
    ret = PRT_SigSuspend(&mask);
    print_result_int("idle task invalid", OS_ERRNO_SIGNAL_TASKID_INVALID, ret);
    RUNNING_TASK->taskPid = 1; // restore

    // Case 5: no pending, partial mask
    mask = sigMask(3);
    ret = PRT_SigSuspend(&mask);
    print_result_int("no pending ok", OS_OK, ret);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
