#include "mock_include/prt_perf.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();

    // 1) no filter -> TRUE
    g_perfCb.taskIdsNr = 0;
    TU_EXPECT_TRUE(OsPerfTaskFilter(10), "No filter allows all");

    // 2) single id match
    g_perfCb.taskIdsNr = 1;
    g_perfCb.taskIds[0] = 123;
    TU_EXPECT_TRUE(OsPerfTaskFilter(123), "Single match");

    // 3) single id mismatch
    TU_EXPECT_TRUE(!OsPerfTaskFilter(124), "Single mismatch");

    // 4) multiple ids contain
    g_perfCb.taskIdsNr = 3;
    g_perfCb.taskIds[0] = 1;
    g_perfCb.taskIds[1] = 2;
    g_perfCb.taskIds[2] = 3;
    TU_EXPECT_TRUE(OsPerfTaskFilter(2), "Multiple match");

    // 5) multiple ids not contain
    TU_EXPECT_TRUE(!OsPerfTaskFilter(5), "Multiple not contain");

    TU_report();
    return 0;
}