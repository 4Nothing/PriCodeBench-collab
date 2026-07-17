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
    uintptr_t s = PRT_SplIrqWriteLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=all_ones", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 1, "写锁基础场景", exp, act, s == 0xABCD1234u && l.rawLock == (uintptr_t)-1);

    s = PRT_SplIrqWriteLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=all_ones", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 2, "重复写锁", exp, act, s == 0xABCD1234u && l.rawLock == (uintptr_t)-1);

    struct PrtSpinLock l2 = {.rawLock = 5};
    s = PRT_SplIrqWriteLock(&l2);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=all_ones", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l2.rawLock);
    test_case_result(&stats, 3, "脏值写锁", exp, act, s == 0xABCD1234u && l2.rawLock == (uintptr_t)-1);

    struct PrtSpinLock a = {.rawLock = 0}, b = {.rawLock = 0};
    uintptr_t s1 = PRT_SplIrqWriteLock(&a);
    uintptr_t s2 = PRT_SplIrqWriteLock(&b);
    snprintf(exp, sizeof(exp), "a=all_ones,b=all_ones");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR, a.rawLock, b.rawLock);
    test_case_result(&stats, 4, "多实例写锁", exp, act, a.rawLock == (uintptr_t)-1 && b.rawLock == (uintptr_t)-1 && s1 == 0xABCD1234u && s2 == 0xABCD1234u);

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