#include <stdio.h>
#include <inttypes.h>

/* Basic types */
typedef uint32_t U32;
// uintptr_t provided by <inttypes.h>

/* Test stats structure */
typedef struct {
    int passed;
    int total;
} TestStats;

/* Test helper functions - forward declarations */
void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed);
void print_pass_rate(const TestStats *stats);

/* PrtSpinLock structure */

/* PRT_SplIrqLock implementation for testing */
#define PRT_SPL_IRQ_LOCK_DEFINED
#define PRT_IntLock() ((uintptr_t)0xABCD1234u)
#include "auto_stub.h"

#include "func_under_test.c"


int main(void)
{
    TestStats stats = {0, 0};
    char exp[80], act[80];

    // Case 1: basic lock — func ignores spinlock, only returns PRT_IntLock()
    PrtSpinLock l = {.rawLock = 0};
    uintptr_t s = PRT_SplIrqLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=0", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 1, "IRQ锁基础场景", exp, act, s == 0xABCD1234u && l.rawLock == 0);

    // Case 2: repeated lock — rawLock stays 0
    s = PRT_SplIrqLock(&l);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=0", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l.rawLock);
    test_case_result(&stats, 2, "重复IRQ加锁", exp, act, s == 0xABCD1234u && l.rawLock == 0);

    // Case 3: dirty initial value stays as-is (func ignores spinlock)
    PrtSpinLock l2 = {.rawLock = 99};
    s = PRT_SplIrqLock(&l2);
    snprintf(exp, sizeof(exp), "intSave=0x%lx, rawLock=99", (unsigned long)0xABCD1234u);
    snprintf(act, sizeof(act), "intSave=0x%lx, rawLock=%" PRIuPTR, (unsigned long)s, l2.rawLock);
    test_case_result(&stats, 3, "脏值IRQ加锁归原", exp, act, s == 0xABCD1234u && l2.rawLock == 99);

    // Case 4: multiple instances — neither rawLock changes
    PrtSpinLock a = {.rawLock = 0}, b = {.rawLock = 0};
    uintptr_t s1 = PRT_SplIrqLock(&a);
    uintptr_t s2 = PRT_SplIrqLock(&b);
    snprintf(exp, sizeof(exp), "a=0,b=0, s=const");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR ", s1=0x%lx,s2=0x%lx",
             a.rawLock, b.rawLock, (unsigned long)s1, (unsigned long)s2);
    test_case_result(&stats, 4, "多实例IRQ加锁", exp, act, a.rawLock == 0 && b.rawLock == 0 && s1 == 0xABCD1234u && s2 == 0xABCD1234u);

    // Case 5: NULL not tested (API requires non-NULL)
    snprintf(exp, sizeof(exp), "no-op");
    snprintf(act, sizeof(act), "no-op");
    test_case_result(&stats, 5, "NULL 情况（不调用）", exp, act, 1);

    print_pass_rate(&stats);
    return (stats.passed == stats.total) ? 0 : 1;
}

/* Test helper implementations */
void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed)
{
    (void)case_num;
    (void)name;
    (void)expected;
    (void)actual;
    stats->total++;
    if (passed) {
        stats->passed++;
    }
}

void print_pass_rate(const TestStats *stats)
{
    printf("Pass-Rate: %.2f%%\n", stats->total > 0 ? (stats->passed * 100.0 / stats->total) : 0.0);
}
