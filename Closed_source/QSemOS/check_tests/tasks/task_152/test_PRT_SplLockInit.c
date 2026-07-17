#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

/* Basic types */
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;
typedef int32_t S32;

/* Test stats structure */
typedef struct {
    int passed;
    int total;
} TestStats;

/* Test helper functions - forward declarations */
void test_case_result(TestStats *stats, int case_num, const char *name, const char *expected, const char *actual, int passed);
void print_pass_rate(const TestStats *stats);

/* PRT_SplLockInit and PRT_SplLock stub implementation */
struct PrtSpinLock {
    volatile uintptr_t rawLock;
};

#define STRUCT_PRT_SPINLOCK_DEFINED
#define PRT_SPLLOCKINIT_STUB_DEFINED
#include "auto_stub.h"
/* OS section macros needed by func_under_test.c */
#include "func_under_test.c"


int main(void)
{
    TestStats stats = {0, 0};
    char exp[64], act[64];

    // Case 1: NULL pointer -> error
    U32 ret = PRT_SplLockInit(NULL);
    snprintf(exp, sizeof(exp), "ret=%u", (unsigned)OS_ERRNO_SPL_ALLOC_ADDR_INVALID);
    snprintf(act, sizeof(act), "ret=%u", (unsigned)ret);
    test_case_result(&stats, 1, "NULL 指针初始化应报错", exp, act,
                     ret == OS_ERRNO_SPL_ALLOC_ADDR_INVALID);

    // Case 2: Valid lock -> success and rawLock reset to 0
    struct PrtSpinLock l2 = {.rawLock = 123};
    ret = PRT_SplLockInit(&l2);
    snprintf(exp, sizeof(exp), "ret=0, rawLock=0");
    snprintf(act, sizeof(act), "ret=%u, rawLock=%" PRIuPTR, (unsigned)ret, l2.rawLock);
    test_case_result(&stats, 2, "正常初始化", exp, act, ret == 0u && l2.rawLock == 0u);

    // Case 3: Initialize same lock twice -> still success, rawLock stays 0
    l2.rawLock = 5;
    ret = PRT_SplLockInit(&l2);
    snprintf(exp, sizeof(exp), "ret=0, rawLock=0");
    snprintf(act, sizeof(act), "ret=%u, rawLock=%" PRIuPTR, (unsigned)ret, l2.rawLock);
    test_case_result(&stats, 3, "重复初始化", exp, act, ret == 0u && l2.rawLock == 0u);

    // Case 4: Multiple locks
    struct PrtSpinLock a = {.rawLock = (uintptr_t)-1};
    struct PrtSpinLock b = {.rawLock = 99};
    U32 r1 = PRT_SplLockInit(&a);
    U32 r2 = PRT_SplLockInit(&b);
    snprintf(exp, sizeof(exp), "a=0,b=0, r1=0,r2=0");
    snprintf(act, sizeof(act), "a=%" PRIuPTR ",b=%" PRIuPTR ", r1=%u,r2=%u",
             a.rawLock, b.rawLock, (unsigned)r1, (unsigned)r2);
    test_case_result(&stats, 4, "多把锁初始化", exp, act, r1 == 0u && r2 == 0u && a.rawLock == 0u && b.rawLock == 0u);

    // Case 5: Pre-dirty then init -> cleared
    struct PrtSpinLock l3 = {.rawLock = 0xFFFFu};
    ret = PRT_SplLockInit(&l3);
    snprintf(exp, sizeof(exp), "ret=0, rawLock=0");
    snprintf(act, sizeof(act), "ret=%u, rawLock=%" PRIuPTR, (unsigned)ret, l3.rawLock);
    test_case_result(&stats, 5, "脏值初始化后应清零", exp, act, ret == 0u && l3.rawLock == 0u);

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
