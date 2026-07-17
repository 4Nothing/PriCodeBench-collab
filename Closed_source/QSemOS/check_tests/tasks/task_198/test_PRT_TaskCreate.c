#define TEST_UTILS_DEFINE_GLOBALS
#include "test_utils.h"

U32 OsTaskCreateOnly(TskHandle *taskPid, struct TskInitParam *initParam, bool isSmpIdle);
#include "func_under_test.c"


static int total = 0, passed = 0;

static U32 fake_ret = OS_OK;
static int called = 0;
static TskHandle last_id = 0;
static struct TskInitParam last_ip;
U32 __attribute__((weak)) OsTaskCreateOnly(TskHandle *taskPid, struct TskInitParam *initParam, bool isSmpIdle)
{
    (void)isSmpIdle;
    called++;
    last_ip = *initParam;
    *taskPid = 999;
    last_id = *taskPid;
    return fake_ret;
}

static void reset()
{
    fake_ret = OS_OK;
    called = 0;
    last_id = 0;
    memset(&last_ip, 0, sizeof(last_ip));
}

static void case_success_forward()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    U32 r = PRT_TaskCreate(&id, &ip);
    total++;
    passed += run_check("returns fake_ret", OS_OK, r);
    total++;
    passed += run_check("forward called once", 1, called);
    total++;
    passed += run_check("task id set", 999, id);
}

static void case_error_bubbles()
{
    reset();
    fake_ret = 0xB1u;
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    U32 r = PRT_TaskCreate(&id, &ip);
    total++;
    passed += run_check("error bubbled", 0xB1, r);
}

static void case_params_passed()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)0xAA, {1, 2, 3, 4}, 5, "name", OS_TSK_SCHED_RR, 0, 256};
    (void)PRT_TaskCreate(&id, &ip);
    total++;
    passed += run_check("policy passed", OS_TSK_SCHED_RR, last_ip.policy);
    total++;
    passed += run_check("name passed len", 0, strcmp("name", last_ip.name));
}

static void case_multiple_calls()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    (void)PRT_TaskCreate(&id, &ip);
    (void)PRT_TaskCreate(&id, &ip);
    total++;
    passed += run_check("called twice", 2, called);
}

static void case_id_value_propagation()
{
    reset();
    TskHandle id = 0;
    struct TskInitParam ip = {(TskEntryFunc)1, {0}, 1, "t", 0, 0, 64};
    (void)PRT_TaskCreate(&id, &ip);
    total++;
    passed += run_check("id is 999", 999, id);
}

int main()
{
    case_success_forward();
    case_error_bubbles();
    case_params_passed();
    case_multiple_calls();
    case_id_value_propagation();
    double pr = (total ? 100.0 * passed / total : 0.0);
    printf("Pass-Rate: %.2f%%\n", pr);
    return (passed == total) ? 0 : 1;
}
