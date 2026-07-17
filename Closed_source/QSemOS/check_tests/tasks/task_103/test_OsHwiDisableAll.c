#include "test_support.h"
#include "prt_trace.h"
#include "prt_irq_internal.h"
#include "func_under_test.c"

int main(void)
{
    int passed = 0, total = 0;
    OsHwiDescInitAll();

    /* ensure some modes non-invalid; our stub treats all valid, so just run */
    U32 prev = g_disable_count;
    OsHwiDisableAll();
    long long expectedMin = prev + OS_HWI_MAX_NUM; /* approximate upper bound; some may be skipped if check macro changes */
    long long actual = g_disable_count;
    int pass1 = (actual >= expectedMin - 5); /* tolerate small variation due to internal checks */
    print_case_result("bulk disables >= N-5", expectedMin, actual, pass1);
    passed += pass1;
    total++;

    /* sample a few flags */
    unsigned idxs[4] = {0, 1, OS_HWI_MAX_NUM / 2, OS_HWI_MAX_NUM - 1};
    for (int i = 0; i < 4; ++i)
    {
        char name[64];
        snprintf(name, sizeof(name), "disabled bit %u", idxs[i]);
        int pass = (g_disabled_mask[idxs[i]] == 1);
        print_case_result(name, 1, g_disabled_mask[idxs[i]], pass);
        passed += pass;
        total++;
    }

    double rate = (total ? (100.0 * passed / total) : 0.0);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (passed == total) ? 0 : 1;
}
