#include "test_common.h"
#include "stubs/prt_signal.h"
#include "../task_075/func_under_test.c"
#include "../task_083/func_under_test.c"
#include "../task_076/func_under_test.c"
#include "../task_081/func_under_test.c"
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
    g_cnt = 0;

    signalInfo info;

    // Case 1: null info -> invalid
    U32 ret = PRT_SignalDeliver(RUNNING_TASK->taskPid, NULL);
    print_result_int("NULL info invalid", OS_ERRNO_SIGNAL_PARA_INVALID, ret);

    // Case 2: invalid signum -> invalid
    info.si_signo = -1;
    ret = PRT_SignalDeliver(RUNNING_TASK->taskPid, &info);
    print_result_int("invalid signo", OS_ERRNO_SIGNAL_NUM_INVALID, ret);

    // Case 3: deliver to running task with handler -> handled immediately
    info.si_signo = 2;
    RUNNING_TASK->sigVectors[2] = hcount;
    ret = PRT_SignalDeliver(RUNNING_TASK->taskPid, &info);
    print_result_int("deliver to self OK", OS_OK, ret);
    print_result_int("handler ran", 1, g_cnt);
    print_result_int("pending cleared", 0, (RUNNING_TASK->sigPending & sigMask(2)) != 0);

    // Case 4: blocked signal -> remains pending, no handler
    info.si_signo = 4;
    RUNNING_TASK->sigMask = sigMask(4);
    RUNNING_TASK->sigVectors[4] = hcount;
    ret = PRT_SignalDeliver(RUNNING_TASK->taskPid, &info);
    print_result_int("blocked deliver OK", OS_OK, ret);
    print_result_int("handler not run", 1, g_cnt);
    print_result_int("pending has 4", 1, (RUNNING_TASK->sigPending & sigMask(4)) != 0);

    // Case 5: deliver to non-running task path -> set HOLD state
    // Simulate by changing pid in running task check: deliver to pid 2
    info.si_signo = 6;
    RUNNING_TASK->sigMask = 0;
    RUNNING_TASK->sigVectors[6] = hcount;
    ret = PRT_SignalDeliver(2, &info);
    print_result_int("deliver to other pid OK", OS_OK, ret);
    print_result_int("hold flag set or pending kept", 1, ((RUNNING_TASK->taskStatus & OS_TSK_HOLD_SIGNAL) != 0) || ((RUNNING_TASK->sigPending & sigMask(6)) != 0));

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
