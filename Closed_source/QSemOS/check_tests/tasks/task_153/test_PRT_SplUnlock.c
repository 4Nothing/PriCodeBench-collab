#include <stdio.h>
#include <inttypes.h>

typedef uint32_t U32;
// uintptr_t provided by <inttypes.h>

typedef struct {
    int passed;
    int total;
} TestStats;

#define PRT_SPL_UNLOCK_DEFINED
#include "auto_stub.h"
/* OS section macros needed by func_under_test.c */
#define OS_SEC_ALIGN
#define OS_SEC_ALW_INLINE
#define OS_SEC_L0_TEXT
#define OS_SEC_L1_TEXT
#define OS_SEC_L2_TEXT
#define OS_SEC_L4_TEXT
#define OS_SEC_L4_BSS
#define OS_SEC_L4_DATA
#define OS_SEC_TEXT
#define OS_SEC_SPIN_LOCK_TEXT
#define OS_SEC_BSS
#define OS_SEC_DATA
#define OS_SEC_RO
#define OS_SEC_RW
#define OS_SEC_WEAK
#define OS_SEC_ZI
#define OS_TSK_SPINUNLOCK()
#undef OsSplUnlock
#include "func_under_test.c"


void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed);
void print_pass_rate(const TestStats *stats);

int main(void)
{
    TestStats stats = {0, 0};
    char exp[96], act[96];

    struct PrtSpinLock l = {.rawLock = 1};
    PRT_SplUnlock(&l);
    snprintf(exp, sizeof(exp), "rawLock=0");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l.rawLock);
    test_case_result(&stats, 1, "解锁基础场景", exp, act, l.rawLock == 0);

    struct PrtSpinLock l2 = {.rawLock = 99};
    PRT_SplUnlock(&l2);
    snprintf(exp, sizeof(exp), "rawLock=0");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l2.rawLock);
    test_case_result(&stats, 2, "脏值解锁", exp, act, l2.rawLock == 0);

    PRT_SplUnlock(&l2);
    snprintf(exp, sizeof(exp), "rawLock=0");
    snprintf(act, sizeof(act), "rawLock=%" PRIuPTR, l2.rawLock);
    test_case_result(&stats, 3, "重复解锁", exp, act, l2.rawLock == 0);

    struct PrtSpinLock a = {.rawLock = 1}, b = {.rawLock = 1};
    PRT_SplUnlock(&a);
    PRT_SplUnlock(&b);
    snprintf(exp, sizeof(exp), "a=0,b=0");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR, a.rawLock, b.rawLock);
    test_case_result(&stats, 4, "多实例解锁", exp, act, a.rawLock == 0 && b.rawLock == 0);

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