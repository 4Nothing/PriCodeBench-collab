#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

#include "auto_stub.h"
#include "func_under_test.c"


static int total = 0, passed = 0;

static void case_basic_init()
{
    struct TskInitParam ip = {0};
    ip.taskEntry = (TskEntryFunc)0xABC;
    ip.taskPrio = 3;
    ip.policy = OS_TSK_SCHED_FIFO;
    ip.args[0] = 1;
    ip.args[3] = 4;
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    g_timeSliceCycle = 1234;
    OsTskCreateTcbInit(0xDEAD, &ip, 0x1000, 256, &cb);
    total++;
    passed += run_check("stackPointer set", 0xDEAD, (U32)(uintptr_t)cb.stackPointer);
    total++;
    passed += run_check("topOfStack", 0x1000, (U32)cb.topOfStack);
    total++;
    passed += run_check("stackSize", 256, (U32)cb.stackSize);
    total++;
    passed += run_check("priority", 3, cb.priority);
    total++;
    passed += run_check("timeSlice from global", 1234, cb.timeSlice);
}

static void case_policy_rr()
{
    struct TskInitParam ip = {0};
    ip.taskEntry = (TskEntryFunc)1;
    ip.taskPrio = 1;
    ip.policy = OS_TSK_SCHED_RR;
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    g_timeSliceCycle = 5;
    OsTskCreateTcbInit(1, &ip, 2, 3, &cb);
    total++;
    passed += run_check("policy RR", OS_TSK_SCHED_RR, cb.policy);
}

static void case_lists_initialized()
{
    struct TskInitParam ip = {0};
    ip.taskEntry = (TskEntryFunc)1;
    ip.taskPrio = 1;
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    OsTskCreateTcbInit(1, &ip, 2, 3, &cb);
    total++;
    passed += run_check("semBList init", 1, cb.semBList.next == &cb.semBList);
    total++;
    passed += run_check("pendList init", 1, cb.pendList.next == &cb.pendList);
    total++;
    passed += run_check("timerList init", 1, cb.timerList.next == &cb.timerList);
}

static void case_args_copied()
{
    struct TskInitParam ip = {0};
    ip.taskEntry = (TskEntryFunc)1;
    ip.taskPrio = 1;
    ip.args[0] = 10;
    ip.args[1] = 20;
    ip.args[2] = 30;
    ip.args[3] = 40;
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    OsTskCreateTcbInit(0, &ip, 0, 0, &cb);
    total++;
    passed += run_check("arg0", 10, cb.args[0]);
    total++;
    passed += run_check("arg3", 40, cb.args[3]);
}

static void case_locale_set()
{
    struct TskInitParam ip = {0};
    ip.taskEntry = (TskEntryFunc)1;
    ip.taskPrio = 1;
    struct TagTskCb cb;
    memset(&cb, 0, sizeof(cb));
    libc_global_locale = (void *)0xFEED;
    OsTskCreateTcbInit(0, &ip, 0, 0, &cb);
    total++;
    passed += run_check("locale copied", (long long)(uintptr_t)libc_global_locale, (long long)(uintptr_t)cb.locale);
}

int main()
{
    case_basic_init();
    case_policy_rr();
    case_lists_initialized();
    case_args_copied();
    case_locale_set();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}
