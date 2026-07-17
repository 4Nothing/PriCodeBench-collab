#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


static void flush_hook(void *buf, U32 size)
{
    printf("flush called size=%u first=%d\n", size, (int)(size ? (unsigned char)((unsigned char *)buf)[0] : -1));
}

int main(void)
{
    TestStats stats;
    tests_init(&stats);

    // Use internal malloc to ensure buffer is zero-initialized by memset_s
    OsPerfOutPutInit(NULL, 8);
    // Avoid noisy default notify output unless explicitly desired in tests
    OsPerfNotifyHookReg(NULL);

    // 1) No hook registered
    start_capture();
    OsPerfOutPutFlush();
    char *out1 = stop_capture();
    tests_print_case("Flush without hook", "", out1 ? out1 : "(null)", (out1 && strcmp(out1, "") == 0), &stats);
    free(out1);

    // 2) Register hook and flush (size=8)
    OsPerfFlushHookReg(flush_hook);
    start_capture();
    OsPerfOutPutFlush();
    char *out2 = stop_capture();
    tests_print_case("Flush with hook size=8", "flush called size=8 first=0\n", out2 ? out2 : "(null)", (out2 && strcmp(out2, "flush called size=8 first=0\n") == 0), &stats);
    free(out2);

    // 3) Different buffer size (16)
    OsPerfOutPutInit(NULL, 16);
    start_capture();
    OsPerfOutPutFlush();
    char *out3 = stop_capture();
    tests_print_case("Flush with hook size=16", "flush called size=16 first=0\n", out3 ? out3 : "(null)", (out3 && strcmp(out3, "flush called size=16 first=0\n") == 0), &stats);
    free(out3);

    // 4) Unregister hook
    OsPerfFlushHookReg(NULL);
    start_capture();
    OsPerfOutPutFlush();
    char *out4 = stop_capture();
    tests_print_case("Flush after unregister", "", out4 ? out4 : "(null)", (out4 && strcmp(out4, "") == 0), &stats);
    free(out4);

    // 5) Re-register and flush
    OsPerfFlushHookReg(flush_hook);
    start_capture();
    OsPerfOutPutFlush();
    char *out5 = stop_capture();
    tests_print_case("Flush re-register", "flush called size=16 first=0\n", out5 ? out5 : "(null)", (out5 && strcmp(out5, "flush called size=16 first=0\n") == 0), &stats);
    free(out5);

    tests_print_summary(&stats);
    return 0;
}
