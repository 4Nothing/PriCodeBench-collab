#include "support.h"
#include "prt_task_external.h"
#include "prt_sem_external.h"
#include "prt_task_sched_external.h"
#include "func_under_test.c"


U32 g_tskMaxNum = 8;
struct TagTskCb g_mockTcbs[8];
int g_called_OsTskReadyDel = 0;
int g_called_OsTskReadyAdd = 0;
int g_called_OsTskSchedule = 0;

int main(void)
{
    struct TestStats stats = {0};

    /* As OS_OPTION_SEM_PRIO_INHERIT is not defined, OsSemPrioBListUnLock is a no-op; ensure it doesn't crash */

    OsSemPrioBListUnLock(0);
    report_case_u32("unlock flag 0 no-op", 0, 0, &stats);

    OsSemPrioBListUnLock(1);
    report_case_u32("unlock flag 1 no-op", 0, 0, &stats);

    OsSemPrioBListUnLock(2);
    report_case_u32("unlock flag 2 no-op", 0, 0, &stats);

    OsSemPrioBListUnLock(1234);
    report_case_u32("unlock flag 1234 no-op", 0, 0, &stats);

    OsSemPrioBListUnLock((U32)-1);
    report_case_u32("unlock flag UINT32_MAX no-op", 0, 0, &stats);

    report_rate(&stats);
    return 0;
}
