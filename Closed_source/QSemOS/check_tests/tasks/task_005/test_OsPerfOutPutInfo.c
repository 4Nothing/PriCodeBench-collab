#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


int main(void)
{
    TestStats stats;
    tests_init(&stats);

    OsPerfOutPutInit(NULL, 20);
    OsPerfNotifyHookReg(NULL);

    for (int i = 0; i < 5; ++i)
    {
        start_capture();
        OsPerfOutPutInfo();
        char *out = stop_capture();
        // We don't know the pointer value; just check it contains key substrings
        int pass = (out && strstr(out, "dump section data") && strstr(out, "length: 0x14"));
        tests_print_case("Info prints address & length", "contains 'dump section data' and 'length: 0x14'", out ? out : "(null)", pass, &stats);
        free(out);
    }

    tests_print_summary(&stats);
    return 0;
}
