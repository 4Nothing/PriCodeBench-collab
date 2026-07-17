#include "mock_include/prt_perf.h"
#include "test_util.h"
#include "func_under_test.c"

int main(void)
{
    TU_reset();
    Event e = {.eventId = 1, .period = 10, .count = {0}};

    // 1) null event safe
    OsPerfUpdateEventCount(NULL, 1);
    TU_EXPECT_EQ_U32(0, (U32)e.count[0], "Null event no change");

    // 2) add small value
    OsPerfUpdateEventCount(&e, 3);
    TU_EXPECT_EQ_U32(3, (U32)e.count[0], "Increment by 3");

    // 3) add wraps masked to 32 bits
    OsPerfUpdateEventCount(&e, 0xFFFFFFFF);
    TU_EXPECT_EQ_U32(2, (U32)e.count[0], "Mask to low 32 bits");

    // 4) multiple increments
    OsPerfUpdateEventCount(&e, 5);
    OsPerfUpdateEventCount(&e, 7);
    TU_EXPECT_EQ_U32(14, (U32)e.count[0], "Multiple increments");

    // 5) different core would affect other index (not simulated; ensure still index 0)
    TU_EXPECT_EQ_U32(14, (U32)e.count[0], "Index 0 unchanged");

    TU_report();
    return 0;
}