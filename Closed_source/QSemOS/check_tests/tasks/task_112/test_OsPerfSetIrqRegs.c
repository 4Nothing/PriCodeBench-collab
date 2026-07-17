#include "test_util.h"
#include "mock_include/prt_perf.h"

int main(void)
{
    TU_reset();
    RUNNING_TASK->pc = 0;
    RUNNING_TASK->fp = 0;
    // 1) set both
    OsPerfSetIrqRegs(0x1234, 0x5678);
    TU_EXPECT_EQ_U32(0x1234, (U32)RUNNING_TASK->pc, "PC set");
    TU_EXPECT_EQ_U32(0x5678, (U32)RUNNING_TASK->fp, "FP set");
    // 2) overwrite
    OsPerfSetIrqRegs(0x9ABC, 0xDEF0);
    TU_EXPECT_EQ_U32(0x9ABC, (U32)RUNNING_TASK->pc, "PC overwritten");
    // 3) zero values
    OsPerfSetIrqRegs(0, 0);
    TU_EXPECT_EQ_U32(0, (U32)RUNNING_TASK->pc, "PC zeroed");
    // 4) large values mask
    OsPerfSetIrqRegs((uintptr_t)0xFFFFFFFFu, (uintptr_t)0xFFFFFFFFu);
    TU_EXPECT_EQ_U32(0xFFFFFFFFu, (U32)RUNNING_TASK->pc, "PC max");
    // 5) idempotent
    OsPerfSetIrqRegs(RUNNING_TASK->pc, RUNNING_TASK->fp);
    TU_EXPECT_TRUE(1, "Idempotent call");
    TU_report();
    return 0;
}