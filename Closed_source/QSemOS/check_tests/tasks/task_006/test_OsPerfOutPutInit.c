#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


int main(void)
{
    TestStats stats;
    tests_init(&stats);

    // 1) NULL buffer, size 64 (malloc inside)
    U32 ret1 = OsPerfOutPutInit(NULL, 64);
    int remain1 = (ret1 == OS_OK) ? (int)OsPerfOutPutRemainSize() : -1;
    char exp1[64];
    snprintf(exp1, sizeof(exp1), "ret=%u remain=%d", OS_OK, 64);
    char act1[64];
    snprintf(act1, sizeof(act1), "ret=%u remain=%d", ret1, remain1);
    tests_print_case("Init NULL, size=64", exp1, act1, (ret1 == OS_OK && remain1 == 64), &stats);

    // 2) User buffer, size 32
    char buf2[32];
    U32 ret2 = OsPerfOutPutInit(buf2, 32);
    int remain2 = (ret2 == OS_OK) ? (int)OsPerfOutPutRemainSize() : -1;
    char exp2[64];
    snprintf(exp2, sizeof(exp2), "ret=%u remain=%d", OS_OK, 32);
    char act2[64];
    snprintf(act2, sizeof(act2), "ret=%u remain=%d", ret2, remain2);
    tests_print_case("Init user buf, size=32", exp2, act2, (ret2 == OS_OK && remain2 == 32), &stats);

    // 3) size=0 -> error
    char b3[1];
    U32 ret3 = OsPerfOutPutInit(b3, 0);
    char exp3[64];
    snprintf(exp3, sizeof(exp3), "ret=%u", OS_ERROR);
    char act3[64];
    snprintf(act3, sizeof(act3), "ret=%u", ret3);
    tests_print_case("Init size=0", exp3, act3, (ret3 == OS_ERROR), &stats);

    // 4) size=1
    char b4[1];
    U32 ret4 = OsPerfOutPutInit(b4, 1);
    int remain4 = (ret4 == OS_OK) ? (int)OsPerfOutPutRemainSize() : -1;
    char exp4[64];
    snprintf(exp4, sizeof(exp4), "ret=%u remain=%d", OS_OK, 1);
    char act4[64];
    snprintf(act4, sizeof(act4), "ret=%u remain=%d", ret4, remain4);
    tests_print_case("Init size=1", exp4, act4, (ret4 == OS_OK && remain4 == 1), &stats);

    // 5) size=256
    char b5[256];
    U32 ret5 = OsPerfOutPutInit(b5, 256);
    int remain5 = (ret5 == OS_OK) ? (int)OsPerfOutPutRemainSize() : -1;
    char exp5[64];
    snprintf(exp5, sizeof(exp5), "ret=%u remain=%d", OS_OK, 256);
    char act5[64];
    snprintf(act5, sizeof(act5), "ret=%u remain=%d", ret5, remain5);
    tests_print_case("Init size=256", exp5, act5, (ret5 == OS_OK && remain5 == 256), &stats);

    tests_print_summary(&stats);
    return 0;
}
