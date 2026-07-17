#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


int main(void)
{
    TestStats stats;
    tests_init(&stats);

    OsPerfOutPutInit(NULL, 10);
    OsPerfNotifyHookReg(NULL);

    // 1) start remain 10
    {
        char a[32];
        snprintf(a, sizeof(a), "remain=%u", OsPerfOutPutRemainSize());
        tests_print_case("Remain start", "remain=10", a, (OsPerfOutPutRemainSize() == 10), &stats);
    }

    // 2) write 3 -> remain 7
    OsPerfOutPutWrite("abc", 3);
    {
        char a[32];
        snprintf(a, sizeof(a), "remain=%u", OsPerfOutPutRemainSize());
        tests_print_case("Remain after write 3", "remain=7", a, (OsPerfOutPutRemainSize() == 7), &stats);
    }

    // 3) read 2 -> remain 9
    char tmp[10];
    (void)OsPerfOutPutRead(tmp, 2);
    {
        char a[32];
        snprintf(a, sizeof(a), "remain=%u", OsPerfOutPutRemainSize());
        tests_print_case("Remain after read 2", "remain=9", a, (OsPerfOutPutRemainSize() == 9), &stats);
    }

    // 4) write 9 (cap to remain) -> remain 0
    OsPerfOutPutWrite("123456789", 9);
    {
        char a[32];
        snprintf(a, sizeof(a), "remain=%u", OsPerfOutPutRemainSize());
        tests_print_case("Remain after write 9", "remain=0", a, (OsPerfOutPutRemainSize() == 0), &stats);
    }

    // 5) read 10 -> remain 10
    (void)OsPerfOutPutRead(tmp, 10);
    {
        char a[32];
        snprintf(a, sizeof(a), "remain=%u", OsPerfOutPutRemainSize());
        tests_print_case("Remain after read 10", "remain=10", a, (OsPerfOutPutRemainSize() == 10), &stats);
    }

    tests_print_summary(&stats);
    return 0;
}
