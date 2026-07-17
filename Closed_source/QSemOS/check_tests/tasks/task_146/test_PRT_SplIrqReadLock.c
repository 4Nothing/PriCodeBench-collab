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

    struct PrtSpinLock l = {.rawLock = 0};
    uintptr_t s = PRT_SplIrqReadLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=1", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 1, "读锁基础场景", exp, act, s == 0xABCD1234u && l.rawLock == 1);

    s = PRT_SplIrqReadLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=2", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 2, "重复读锁计数递增", exp, act, s == 0xABCD1234u && l.rawLock == 2);

    struct PrtSpinLock l2 = {.rawLock = 5};
    s = PRT_SplIrqReadLock(&l2);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=6", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l2.rawLock);
    test_case_result(&stats, 3, "脏值读锁递增", exp, act, s == 0xABCD1234u && l2.rawLock == 6);

    struct PrtSpinLock a = {.rawLock = 0}, b = {.rawLock = 2};
    uintptr_t s1 = PRT_SplIrqReadLock(&a);
    uintptr_t s2 = PRT_SplIrqReadLock(&b);
    snprintf(exp, sizeof(exp), "a=1,b=3");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR, a.rawLock, b.rawLock);
    test_case_result(&stats, 4, "多实例读锁", exp, act, a.rawLock == 1 && b.rawLock == 3 && s1 == 0xABCD1234u && s2 == 0xABCD1234u);

    struct PrtSpinLock l3 = {.rawLock = 0};
    for (int i = 0; i < 5; ++i)
        (void)PRT_SplIrqReadLock(&l3);
    snprintf(exp, sizeof(exp), "rawLock=5");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l3.rawLock);
    test_case_result(&stats, 5, "多次读锁累计", exp, act, l3.rawLock == 5);

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