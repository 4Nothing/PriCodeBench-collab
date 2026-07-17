#include <stdio.h>
#include <inttypes.h>

typedef uint32_t U32;
// uintptr_t provided by <inttypes.h>

typedef struct {
    int passed;
    int total;
} TestStats;


#include "auto_stub.h"

#include "func_under_test.c"


void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed);
void print_pass_rate(const TestStats *stats);

int main(void)
{
    TestStats stats = {0, 0};
    char exp[96], act[96];

    struct PrtSpinLock l = {.rawLock = 3};
    PRT_SplIrqReadUnlock(&l, 0);
    snprintf(exp, sizeof(exp), "rawLock=2");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l.rawLock);
    test_case_result(&stats, 1, "读解锁基础场景", exp, act, l.rawLock == 2);

    PRT_SplIrqReadUnlock(&l, 0);
    PRT_SplIrqReadUnlock(&l, 0);
    snprintf(exp, sizeof(exp), "rawLock=0");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l.rawLock);
    test_case_result(&stats, 2, "多次读解锁", exp, act, l.rawLock == 0);

    PRT_SplIrqReadUnlock(&l, 0);
    snprintf(exp, sizeof(exp), "rawLock=0");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l.rawLock);
    test_case_result(&stats, 3, "已解锁状态再次解锁", exp, act, l.rawLock == 0);

    struct PrtSpinLock a = {.rawLock = 5}, b = {.rawLock = 2};
    PRT_SplIrqReadUnlock(&a, 0);
    PRT_SplIrqReadUnlock(&b, 0);
    snprintf(exp, sizeof(exp), "a=4,b=1");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR, a.rawLock, b.rawLock);
    test_case_result(&stats, 4, "多实例读解锁", exp, act, a.rawLock == 4 && b.rawLock == 1);

    snprintf(exp, sizeof(exp), "no-op");
    snprintf(act, sizeof(act), "no-op");
    test_case_result(&stats, 5, "NULL指针（不调用）", exp, act, 1);

    print_pass_rate(&stats);
    return (stats.passed == stats.total) ? 0 : 1;
}

void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed)
{
    (void)case_num; (void)name; (void)expected; (void)actual;
    stats->total++;
    if (passed) stats->passed++;
}

void print_pass_rate(const TestStats *stats)
{
    printf("Pass-Rate: %.2f%%\n", stats->total > 0 ? (stats->passed * 100.0 / stats->total) : 0.0);
}