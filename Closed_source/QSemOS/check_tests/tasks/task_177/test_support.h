#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "common.h"
#include <stdio.h>

static void print_pass_rate(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%%\n", rate);
}

#endif /* TEST_SUPPORT_H */
