#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"

void OsHwiDescInitAll(void);
void OsHwiCombineNodeInit(void);

#include "func_under_test.c"

int main(void)
{
    int passed = 0, total = 0;

    /* Case 1: First init returns OS_OK */
    U32 ret1 = OsHwiConfigInit();
    print_case_result("return OS_OK", OS_OK, ret1, ret1 == OS_OK);
    passed += (ret1 == OS_OK);
    total++;

    /* Case 2: Descriptors initialized for 0 */
    long long expPtr = (long long)(uintptr_t)OsHwiDefaultHandler;
    long long actPtr = (long long)(uintptr_t)OsHwiFuncGet(0);
    print_case_result("desc[0] default", expPtr, actPtr, expPtr == actPtr);
    passed += (expPtr == actPtr);
    total++;

    /* Case 3: Descriptors initialized for mid */
    U32 mid = (OS_HWI_MAX_NUM / 2);
    actPtr = (long long)(uintptr_t)OsHwiFuncGet(mid);
    print_case_result("desc[mid] default", expPtr, actPtr, expPtr == actPtr);
    passed += (expPtr == actPtr);
    total++;

    /* Case 4: Re-init is idempotent */
    U32 ret2 = OsHwiConfigInit();
    print_case_result("reinit OS_OK", OS_OK, ret2, ret2 == OS_OK);
    passed += (ret2 == OS_OK);
    total++;

    /* Case 5: After reinit still default at last */
    U32 last = OS_HWI_MAX_NUM - 1;
    actPtr = (long long)(uintptr_t)OsHwiFuncGet(last);
    print_case_result("desc[last] default", expPtr, actPtr, expPtr == actPtr);
    passed += (expPtr == actPtr);
    total++;

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}
