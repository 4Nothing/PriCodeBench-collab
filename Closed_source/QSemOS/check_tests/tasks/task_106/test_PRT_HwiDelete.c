#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"
#include "../task_101/func_under_test.c"
#include "../task_102/func_under_test.c"
#include "../task_104/func_under_test.c"

U32 OsHwiConnectHandle(U32 irqNum, HwiProcFunc handler, HwiArg arg);

#include "../task_105/func_under_test.c"

U32 OsHwiDeleteFormResume(U32 irqNum);

#include "func_under_test.c"

static void isr(HwiArg a) { (void)a; }

int main(void)
{
    int passed = 0, total = 0;
    OsHwiDescInitAll();

    /* 1. Invalid hwi num */
    U32 ret = PRT_HwiDelete(OS_HWI_MAX_NUM);
    print_case_result("invalid hwi num", OS_ERRNO_HWI_NUM_INVALID, ret, ret == OS_ERRNO_HWI_NUM_INVALID);
    passed += (ret == OS_ERRNO_HWI_NUM_INVALID);
    total++;

    /* 2. Delete on default handler (should reset to default, OK) */
    ret = PRT_HwiDelete(0);
    print_case_result("delete default ok", OS_OK, ret, ret == OS_OK);
    passed += (ret == OS_OK);
    total++;

    /* 3. Create and delete */
    (void)PRT_HwiCreate(3, isr, 0);
    ret = PRT_HwiDelete(3);
    print_case_result("delete created ok", OS_OK, ret, ret == OS_OK);
    passed += (ret == OS_OK);
    total++;
    long long expPtr = (long long)(uintptr_t)OsHwiDefaultHandler;
    long long actPtr = (long long)(uintptr_t)OsHwiFuncGet(3);
    print_case_result("after delete default", expPtr, actPtr, expPtr == actPtr);
    passed += (expPtr == actPtr);
    total++;

    /* 4. Check disable invoked */
    U32 prevDisableCount = g_disable_count;
    (void)PRT_HwiDelete(1);
    long long expectedDisables = prevDisableCount + 1;
    long long actualDisables = g_disable_count;
    print_case_result("disable called", expectedDisables, actualDisables, expectedDisables == actualDisables);
    passed += (expectedDisables == actualDisables);
    total++;

    /* 5. Edge hwi */
    (void)PRT_HwiCreate(OS_HWI_MAX_NUM - 1, isr, 0);
    ret = PRT_HwiDelete(OS_HWI_MAX_NUM - 1);
    print_case_result("delete edge ok", OS_OK, ret, ret == OS_OK);
    passed += (ret == OS_OK);
    total++;

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}
