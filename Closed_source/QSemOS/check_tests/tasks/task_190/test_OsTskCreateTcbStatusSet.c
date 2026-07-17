#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void reset()
{
    stub_spin_lock_count = 0;
    stub_spin_unlock_count = 0;
}

static void case_sets_flags()
{
    reset();
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    struct TskInitParam ip = {0};
    OsTskCreateTcbStatusSet(&cb, &ip);
    total++;
    passed += run_check("status set", OS_TSK_SUSPEND | OS_TSK_INUSE, cb.taskStatus);
}

static void case_locks_called()
{
    reset();
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    struct TskInitParam ip = {0};
    OsTskCreateTcbStatusSet(&cb, &ip);
    total++;
    passed += run_check("lock called", 1, stub_spin_lock_count == 1);
    total++;
    passed += run_check("unlock called", 1, stub_spin_unlock_count == 1);
}

static void case_idempotent()
{
    reset();
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    struct TskInitParam ip = {0};
    OsTskCreateTcbStatusSet(&cb, &ip);
    U32 first = cb.taskStatus;
    OsTskCreateTcbStatusSet(&cb, &ip);
    total++;
    passed += run_check("status unchanged", first, cb.taskStatus);
}

static void case_initial_status_ignored()
{
    reset();
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    cb.taskStatus = 0xFFFF;
    struct TskInitParam ip = {0};
    OsTskCreateTcbStatusSet(&cb, &ip);
    total++;
    passed += run_check("overwritten", OS_TSK_SUSPEND | OS_TSK_INUSE, cb.taskStatus);
}

static void case_with_different_initParam()
{
    reset();
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    struct TskInitParam ip = {.policy = OS_TSK_SCHED_RR};
    OsTskCreateTcbStatusSet(&cb, &ip);
    total++;
    passed += run_check("status independent of param", OS_TSK_SUSPEND | OS_TSK_INUSE, cb.taskStatus);
}

int main()
{
    case_sets_flags();
    case_locks_called();
    case_idempotent();
    case_initial_status_ignored();
    case_with_different_initParam();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}
