#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define OS_SEC_L2_TEXT

uintptr_t PRT_HwiLock() { return 0; }

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
    for (int i = 0; i < 5; ++i)
    {
        uintptr_t v = OsLogLockOn();
        char name[64];
        snprintf(name, sizeof(name), "lock call %d", i + 1);
        print_result(name, 0, (long)v, &pass, &total);
    }
    finish_report(pass, total);
    return pass == total ? 0 : 1;
}