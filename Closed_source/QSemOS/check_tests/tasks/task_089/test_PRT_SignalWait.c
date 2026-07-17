#include "test_common.h"
#include "stubs/prt_signal.h"
#include "func_under_test.c"

static inline void add_pending_signal(struct TagTskCb *taskCb, int sig)
{
    signalInfo info;
    info.si_signo = sig;
    info.si_code = 0;
    (void)OsAddSignalPendingFlag(taskCb, &info);
}

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    signalInfo out;
    signalSet set = sigMask(1) | sigMask(3);

    // Case 1: null set -> invalid
    U32 ret = PRT_SignalWait(NULL, &out, 1);
    print_result_int("null set invalid", OS_ERRNO_SIGNAL_PARA_INVALID, ret);

    // Case 2: empty set -> invalid
    signalSet empty = 0;
    ret = PRT_SignalWait(&empty, &out, 1);
    print_result_int("empty set invalid", OS_ERRNO_SIGNAL_PARA_INVALID, ret);

    // Case 3: null info -> invalid
    ret = PRT_SignalWait(&set, NULL, 1);
    print_result_int("null info invalid", OS_ERRNO_SIGNAL_PARA_INVALID, ret);

    // Case 4: pending exists -> consumed immediately
    add_pending_signal(RUNNING_TASK, 3);
    memset(&out, 0, sizeof(out));
    ret = PRT_SignalWait(&set, &out, 100);
    print_result_int("pending immediate OK", OS_OK, ret);
    print_result_int("got 3", 3, out.si_signo);

    // Case 5: no pending and timeout 0 -> invalid
    ret = PRT_SignalWait(&set, &out, 0);
    print_result_int("timeout=0 invalid", OS_ERRNO_SIGNAL_PARA_INVALID, ret);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
