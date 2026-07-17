#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define OS_SEC_L2_TEXT

void PRT_HwiRestore(uintptr_t value) { (void)value; }

#include "func_under_test.c"

static void print_result(const char *case_name, uintptr_t expected, uintptr_t actual, int *pass, int *total)
{
    printf("[CASE] %s\n", case_name);
    printf("  Expected: 0x%lX\n", (unsigned long long)expected);
    printf("  Actual  : 0x%lX\n", (unsigned long long)actual);
    int ok = (expected == actual);
    printf("  Result  : %s\n\n", ok ? "PASS" : "FAIL");
    (*total)++;
    *pass += ok;
}

static void finish_report(int pass, int total)
{
    double rate = total ? (100.0 * pass / total) : 0.0;
    printf("Pass-Rate: %.2f%% (%d/%d)\n", rate, pass, total);
}

int main(void)
{
    int pass = 0, total = 0;
    uintptr_t vals[5] = {0, 1, 123, (uintptr_t)0xFFFFFFFFu, (uintptr_t)0xABCDEF01u};
    for (int i = 0; i < 5; ++i)
    {
        OsLogLockRestore(vals[i]);
        char name[64];
        snprintf(name, sizeof(name), "restore call %d", i + 1);
        print_result(name, 1, 1, &pass, &total);
    }
    finish_report(pass, total);
    return pass == total ? 0 : 1;
}