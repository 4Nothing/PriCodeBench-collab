#include "test_utils.h"
#include "prt_perf_output.h"
#include "func_under_test.c"
#include <stdio.h>
#include <string.h>


static int flush_called = 0;
static void flush_hook(void *buf, U32 size)
{
    (void)buf;
    (void)size;
    flush_called++;
}

int main(void)
{
    TestStats stats;
    tests_init(&stats);

    OsPerfOutPutInit(NULL, 8);

    // 1) no hook -> not called
    OsPerfOutPutFlush();
    {
        char act[32];
        snprintf(act, sizeof(act), "called=%d", flush_called);
        tests_print_case("No flush hook", "called=0", act, (flush_called == 0), &stats);
    }

    // 2) register
    OsPerfFlushHookReg(flush_hook);
    OsPerfOutPutFlush();
    {
        char act[32];
        snprintf(act, sizeof(act), "called=%d", flush_called);
        tests_print_case("Flush hook once", "called=1", act, (flush_called == 1), &stats);
    }

    // 3) multiple times
    OsPerfOutPutFlush();
    OsPerfOutPutFlush();
    {
        char act[32];
        snprintf(act, sizeof(act), "called=%d", flush_called);
        tests_print_case("Flush hook thrice", "called=3", act, (flush_called == 3), &stats);
    }

    // 4) unregister
    OsPerfFlushHookReg(NULL);
    OsPerfOutPutFlush();
    {
        char act[32];
        snprintf(act, sizeof(act), "called=%d", flush_called);
        tests_print_case("Flush hook unregistered", "called=3", act, (flush_called == 3), &stats);
    }

    // 5) re-register
    OsPerfFlushHookReg(flush_hook);
    OsPerfOutPutFlush();
    {
        char act[32];
        snprintf(act, sizeof(act), "called=%d", flush_called);
        tests_print_case("Flush hook re-registered", "called=4", act, (flush_called == 4), &stats);
    }

    tests_print_summary(&stats);
    return 0;
}
