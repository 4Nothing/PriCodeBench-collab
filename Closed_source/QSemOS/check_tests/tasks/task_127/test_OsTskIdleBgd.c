#define _GNU_SOURCE
#include "common/test_common.h"
#include "common/shim_prt_task_test.h"

#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include "func_under_test.c"


static atomic_int loops = 0;

static void counting_sleep(void)
{
    loops++;
    // tiny yield to avoid hogging
    usleep(1000);
}

static void reset_env(void)
{
    test_reset_stats();
    shim_reset_instrumentation();
    g_taskCoreSleep = counting_sleep;
    loops = 0;
}

static void *bgd_runner(void *arg)
{
    (void)arg;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    // Run for a limited time then cancel from outside
    OsTskIdleBgd();
    return NULL;
}

int main(void)
{
    reset_env();

    // Case 1: thread runs and increments loops
    pthread_t th;
    pthread_create(&th, NULL, bgd_runner, NULL);
    usleep(20 * 1000); // 20ms
    int c1 = loops;
    EXPECT_BOOL_EQ("loop progressed >0", true, c1 > 0);
    pthread_cancel(th);
    pthread_join(th, NULL);

    // Case 2: change to NULL should spin but no increments; run very briefly
    g_taskCoreSleep = NULL;
    loops = 0;
    pthread_create(&th, NULL, bgd_runner, NULL);
    usleep(5 * 1000); // 5ms
    int c2 = loops;
    EXPECT_INT_EQ("no increments when NULL", 0, c2);
    pthread_cancel(th);
    pthread_join(th, NULL);

    // Case 3: counting again
    g_taskCoreSleep = counting_sleep;
    loops = 0;
    pthread_create(&th, NULL, bgd_runner, NULL);
    usleep(10 * 1000);
    int c3 = loops;
    EXPECT_BOOL_EQ("increments after restore", true, c3 > 0);
    pthread_cancel(th);
    pthread_join(th, NULL);

    // Case 4: multiple short runs
    int accum = 0;
    for (int i = 0; i < 3; ++i)
    {
        loops = 0;
        pthread_create(&th, NULL, bgd_runner, NULL);
        usleep(5 * 1000);
        accum += loops;
        pthread_cancel(th);
        pthread_join(th, NULL);
    }
    EXPECT_BOOL_EQ("accum > 0 after cycles", true, accum > 0);

    // Case 5: ensure cancel works even when hook NULL
    g_taskCoreSleep = NULL;
    loops = 0;
    pthread_create(&th, NULL, bgd_runner, NULL);
    usleep(3 * 1000);
    int c5 = loops;
    EXPECT_INT_EQ("still zero when NULL", 0, c5);
    pthread_cancel(th);
    pthread_join(th, NULL);

    test_print_summary();
    return 0;
}
