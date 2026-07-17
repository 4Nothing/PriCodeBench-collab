#include "test_common.h"
#include "stubs/prt_signal.h"
#include "../task_079/func_under_test.c"
#include "func_under_test.c"

static void h1(int s) { (void)s; }

int main(void)
{
    RESET_COUNTERS();
    StubResetTask();

    struct _sigaction act, old;

    // Case 1: invalid signum
    U32 ret = PRT_SignalAction(-1, &act, &old);
    print_result_int("invalid signum", OS_ERRNO_SIGNAL_NUM_INVALID, ret);

    // Case 2: set handler
    act.saHandler = h1;
    ret = PRT_SignalAction(3, &act, NULL);
    print_result_int("set handler ok", OS_OK, ret);
    print_result_ptr("vector is h1", h1, RUNNING_TASK->sigVectors[3]);

    // Case 3: get old
    ret = PRT_SignalAction(3, NULL, &old);
    print_result_int("get old ok", OS_OK, ret);
    print_result_ptr("old is h1", h1, old.saHandler);

    // Case 4: set IGN -> vector NULL
    act.saHandler = SIG_IGN;
    ret = PRT_SignalAction(3, &act, NULL);
    print_result_int("set IGN ok", OS_OK, ret);
    print_result_ptr("vector is NULL", NULL, RUNNING_TASK->sigVectors[3]);

    // Case 5: set DFL -> default handler
    act.saHandler = SIG_DFL;
    ret = PRT_SignalAction(3, &act, NULL);
    print_result_int("set DFL ok", OS_OK, ret);
    print_result_ptr("vector is default", OsSigDefaultHandler, RUNNING_TASK->sigVectors[3]);

    print_pass_rate();
    return (g_pass == g_total) ? 0 : 1;
}
