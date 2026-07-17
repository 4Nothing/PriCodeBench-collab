#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "prt_perf_pmu.h"

/* Test counters for simple test framework */
int tests_run = 0;
int tests_passed = 0;

/* print_rate - print test pass rate */
void print_rate(void)
{
    if (tests_run == 0) {
        printf("No tests run.\n");
        return;
    }
    printf("Pass rate: %d/%d (%d%%)\n",
           tests_passed, tests_run,
           (tests_passed * 100) / tests_run);
}