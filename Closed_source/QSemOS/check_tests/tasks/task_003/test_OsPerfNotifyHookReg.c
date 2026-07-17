#include "test_utils.h"
#include "prt_perf_output.h"
#include <stdio.h>
#include <string.h>
#include "func_under_test.c"


static int notified = 0;
static void notify_hook(void) { notified++; }

int main(void)
{
    TestStats stats;
    tests_init(&stats);

    OsPerfOutPutInit(NULL, 12); // watermark 12/4=3

    // 1) no hook -> no output
    OsPerfNotifyHookReg(NULL);
    OsPerfOutPutWrite("ab", 2);
    OsPerfOutPutWrite("cd", 1);
    {
        char act[32];
        snprintf(act, sizeof(act), "notified=%d", notified);
        tests_print_case("No hook no notify", "notified=0", act, (notified == 0), &stats);
    }

    // 2) register hook then cross watermark
    OsPerfNotifyHookReg(notify_hook);
    OsPerfOutPutWrite("ef", 2); // cumulative >=3
    {
        char act[32];
        snprintf(act, sizeof(act), "notified=%d", notified);
        tests_print_case("Hook registered", "notified>=1", act, (notified >= 1), &stats);
    }

    // 3) register again same hook
    OsPerfNotifyHookReg(notify_hook);
    OsPerfOutPutWrite("ghij", 4);
    {
        char act[32];
        snprintf(act, sizeof(act), "notified=%d", notified);
        tests_print_case("Hook persists", "notified>=1", act, (notified >= 1), &stats);
    }

    // 4) unregister
    OsPerfNotifyHookReg(NULL);
    int before = notified;
    OsPerfOutPutWrite("klm", 3);
    {
        char act[32];
        snprintf(act, sizeof(act), "notified=%d", notified);
        tests_print_case("Hook unregistered", "notified unchanged", act, (notified == before), &stats);
    }

    // 5) re-register (reset buffer to ensure space) then trigger
    before = notified;
    OsPerfOutPutInit(NULL, 12);
    OsPerfNotifyHookReg(notify_hook);
    OsPerfOutPutWrite("nopq", 4);
    {
        char act[32];
        snprintf(act, sizeof(act), "notified=%d", notified);
        tests_print_case("Hook re-registered", "notified increased", act, (notified > before), &stats);
    }

    tests_print_summary(&stats);
    return 0;
}
