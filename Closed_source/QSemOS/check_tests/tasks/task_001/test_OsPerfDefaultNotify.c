#include "func_under_test.c"
#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>

// Include the C file to access static functions

int main(void)
{
    TestStats stats;
    tests_init(&stats);

    for (int i = 1; i <= 5; ++i)
    {
        start_capture();
        OsPerfDefaultNotify();
        char *out = stop_capture();
        const char *exp = "perf buf waterline notify!\n";
        char case_name[64];
        snprintf(case_name, sizeof(case_name), "OsPerfDefaultNotify #%d", i);
        tests_print_case(case_name, exp, out ? out : "(null)", (out && strcmp(out, exp) == 0), &stats);
        free(out);
    }

    tests_print_summary(&stats);
    return 0;
}
