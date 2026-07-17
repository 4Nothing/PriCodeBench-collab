#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


int main(void)
{
    TestStats stats;
    tests_init(&stats);

    OsPerfOutPutInit(NULL, 16);
    OsPerfNotifyHookReg(NULL);

    char tmp[32];

    // 1) read from empty -> 0
    U32 r1 = OsPerfOutPutRead(tmp, 5);
    char exp1[64];
    snprintf(exp1, sizeof(exp1), "ret=%d data=", 0);
    char act1[64];
    snprintf(act1, sizeof(act1), "ret=%u data=", r1);
    tests_print_case("Read empty", exp1, act1, (r1 == 0), &stats);

    // 2) write "abc", read 2 -> "ab"
    OsPerfOutPutWrite("abc", 3);
    memset(tmp, 0, sizeof(tmp));
    U32 r2 = OsPerfOutPutRead(tmp, 2);
    char exp2[64];
    snprintf(exp2, sizeof(exp2), "ret=2 data=ab");
    char act2[64];
    snprintf(act2, sizeof(act2), "ret=%u data=%.*s", r2, (int)r2, tmp);
    tests_print_case("Read 2 of 'abc'", exp2, act2, (r2 == 2 && strncmp(tmp, "ab", 2) == 0), &stats);

    // 3) read remaining 1 -> "c"
    memset(tmp, 0, sizeof(tmp));
    U32 r3 = OsPerfOutPutRead(tmp, 10);
    char exp3[64];
    snprintf(exp3, sizeof(exp3), "ret=1 data=c");
    char act3[64];
    snprintf(act3, sizeof(act3), "ret=%u data=%.*s", r3, (int)r3, tmp);
    tests_print_case("Read rest of 'abc'", exp3, act3, (r3 == 1 && strncmp(tmp, "c", 1) == 0), &stats);

    // 4) write 10 bytes, read 8
    OsPerfOutPutWrite("0123456789", 10);
    memset(tmp, 0, sizeof(tmp));
    U32 r4 = OsPerfOutPutRead(tmp, 8);
    {
        char act2[64];
        snprintf(act2, sizeof(act2), "ret=%u data=%.*s", r4, (int)r4, tmp);
        tests_print_case("Read 8 of 10", "ret=8 data=01234567", act2, (r4 == 8 && strncmp(tmp, "01234567", 8) == 0), &stats);
    }

    // 5) read remaining (should be 2)
    memset(tmp, 0, sizeof(tmp));
    U32 r5 = OsPerfOutPutRead(tmp, 10);
    char exp5[64];
    snprintf(exp5, sizeof(exp5), "ret=2 data=89");
    char act5[64];
    snprintf(act5, sizeof(act5), "ret=%u data=%.*s", r5, (int)r5, tmp);
    tests_print_case("Read remaining 2", exp5, act5, (r5 == 2 && strncmp(tmp, "89", 2) == 0), &stats);

    tests_print_summary(&stats);
    return 0;
}
