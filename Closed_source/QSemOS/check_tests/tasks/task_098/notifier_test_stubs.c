#include <stdio.h>
#include <string.h>

/* Common pretty-printer for a single case */
void print_case_result(const char *name, int expected, int actual, int *passCnt, int *caseIdx)
{
    printf("[CASE %d] %s\n", *caseIdx, name);
    printf("  Expect: %d\n", expected);
    printf("  Actual: %d\n", actual);
    if (expected == actual)
    {
        (*passCnt)++;
        printf("  Result: PASS\n\n");
    }
    else
    {
        printf("  Result: FAIL\n\n");
    }
    (*caseIdx)++;
}
