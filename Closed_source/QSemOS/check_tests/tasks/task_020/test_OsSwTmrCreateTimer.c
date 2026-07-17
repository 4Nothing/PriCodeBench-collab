#define SKIP_OsSwTmrCreateTimer
#define SKIP_OsSwTmrCreateTimerParaChk
#include "prt_swtmr_internal.h"
#include "test_util.h"

// Semi-real stub for OsSwTmrCreateTimerParaChk
static U32 OsSwTmrCreateTimerParaChk(struct TimerCreatePara *createPara)
{
    if (createPara->callBackFunc == NULL) {
        return OS_ERRNO_TIMER_PROC_FUNC_NULL;
    }
    if (createPara->timerGroupId != OS_TICK_SWTMR_GROUP_ID) {
        return OS_ERRNO_TIMERGROUP_ID_INVALID;
    }
    return OS_OK;
}

#include "func_under_test.c"

static void cb(TimerHandle a1, U32 a2, U32 a3, U32 a4, U32 a5) { (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; }

int main(void)
{
    TU_reset();

    // Prepare a small free list of 2 nodes
    static struct TagSwTmrCtrl nodes[2];
    nodes[0].swtmrIndex = 0;
    nodes[0].next = &nodes[1];
    nodes[0].state = OS_TIMER_FREE;
    nodes[1].swtmrIndex = 1;
    nodes[1].next = NULL;
    nodes[1].state = OS_TIMER_FREE;
    g_tmrFreeList.freeList = &nodes[0];

    struct TimerCreatePara p = {0};
    p.callBackFunc = cb;
    p.mode = 1;
    p.interval = 100;
    p.timerGroupId = OS_TICK_SWTMR_GROUP_ID;

    TimerHandle h = 0;
    U32 r;

    // Case 1: success create first
    r = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("create ok", OS_OK, r);
    TU_check_u32("handle=0x10000000", OS_SWTMR_INDEX_2_ID(0), h);

    // Case 2: success create second
    r = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("create ok 2", OS_OK, r);
    TU_check_u32("handle=0x10000001", OS_SWTMR_INDEX_2_ID(1), h);

    // Case 3: free list exhausted
    r = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("max size", OS_ERRNO_SWTMR_MAXSIZE, r);

    // Case 4: invalid params caught by para chk (cb null)
    p.callBackFunc = NULL;
    r = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("param err", OS_ERRNO_TIMER_PROC_FUNC_NULL, r);

    // Case 5: wrong group id
    p.callBackFunc = cb;
    p.timerGroupId = 99;
    r = OsSwTmrCreateTimer(&p, &h);
    TU_check_u32("group id err", OS_ERRNO_TIMERGROUP_ID_INVALID, r);

    TU_report();
    return 0;
}
