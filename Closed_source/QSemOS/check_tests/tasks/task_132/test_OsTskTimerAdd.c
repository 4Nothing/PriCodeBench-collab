#include "common/test_common.h"
#include "common/shim_prt_task_test.h"
#include "func_under_test.c"

static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
}

static struct TagTskCb make_task(int core, int pid)
{
    struct TagTskCb t = {0};
    t.coreID = core;
    t.taskPid = pid;
    t.isOnRq = false;
    t.taskStatus = 0;
    t.timeSlice = 0;
    // timerList node must be self-clean; nothing else needed
    return t;
}

// Iterate and gather expiration ticks into array
static int collect_ticks(uintptr_t *out, int max)
{
    int n = 0;
    struct TagTskCb *iter = NULL;
    struct TagListObject *lst = &g_testDelayBase->tskList;
    LIST_FOR_EACH(iter, lst, struct TagTskCb, timerList)
    {
        if (n < max)
            out[n] = iter->expirationTick;
        n++;
    }
    return n;
}

int main(void)
{
    reset_env();
    uintptr_t arr[8] = {0};

    // Case 1: insert into empty -> single element with tick g_uniTicks+5
    struct TagTskCb t1 = make_task(0, 41);
    g_uniTicks = 100;
    OsTskTimerAdd(&t1, 5);
    int n1 = collect_ticks(arr, 8);
    EXPECT_INT_EQ("len=1 after first", 1, n1);
    EXPECT_UINT_EQ("tick saved", 105, arr[0]);
    EXPECT_INT_EQ("refresh called once", 1, g_calls_OsTskDlyNearestTicksRefresh);

    // Case 2: insert with smaller tick to front
    struct TagTskCb t2 = make_task(0, 42);
    g_uniTicks = 100;      // keep base
    OsTskTimerAdd(&t2, 2); // 102
    int n2 = collect_ticks(arr, 8);
    EXPECT_INT_EQ("len=2", 2, n2);
    EXPECT_UINT_EQ("first is 102", 102, arr[0]);
    EXPECT_UINT_EQ("second is 105", 105, arr[1]);

    // Case 3: insert in the middle
    struct TagTskCb t3 = make_task(0, 43);
    OsTskTimerAdd(&t3, 4); // 104
    int n3 = collect_ticks(arr, 8);
    EXPECT_INT_EQ("len=3", 3, n3);
    EXPECT_UINT_EQ("order 102,104,105", 102, arr[0]);
    EXPECT_UINT_EQ("order 102,104,105", 104, arr[1]);
    EXPECT_UINT_EQ("order 102,104,105", 105, arr[2]);

    // Case 4: insert equal tick goes after existing equal or before? source adds before first greater, so equal should go after equal runs to end, then add before tskDelay (which will be the first > or end), effectively after equal values. We check stability for equal.
    struct TagTskCb t4 = make_task(0, 44);
    OsTskTimerAdd(&t4, 4); // 104 equal
    int n4 = collect_ticks(arr, 8);
    EXPECT_INT_EQ("len=4", 4, n4);
    EXPECT_UINT_EQ("first 102", 102, arr[0]);
    EXPECT_UINT_EQ("second 104", 104, arr[1]);
    EXPECT_UINT_EQ("third 104", 104, arr[2]);
    EXPECT_UINT_EQ("fourth 105", 105, arr[3]);

    // Case 5: larger tick appended at end
    struct TagTskCb t5 = make_task(0, 45);
    OsTskTimerAdd(&t5, 20); // 120
    int n5 = collect_ticks(arr, 8);
    EXPECT_INT_EQ("len=5", 5, n5);
    EXPECT_UINT_EQ("last 120", 120, arr[4]);

    test_print_summary();
    return 0;
}
