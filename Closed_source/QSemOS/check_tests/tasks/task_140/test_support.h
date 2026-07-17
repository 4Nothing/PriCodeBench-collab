#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "auto_stub.h"

/* Timer globals */
extern volatile U64 g_uniTicks;
extern struct TagSwTmrCtrl *g_swtmrCbArray;
extern U32 g_swTmrMaxNum;
extern struct TagSwTmrCtrl *g_tmrFreeList;
extern struct TickModInfo g_tickModInfo;

/* Task globals */
extern struct TagTskCb g_taskA;
extern struct TagTskCb g_taskB;
extern struct TagTskCb *g_highestTask;
extern uintptr_t g_lastContextLoadPtr;
extern uint32_t g_contextLoadCount;
extern uint32_t g_switchHookCount;
extern uint32_t g_lastPrevPid;
extern uint32_t g_lastNextPid;
extern void (*g_tickDispatcher)(void);

struct TestStats
{
    int total;
    int pass;
};

void Test_InitWheel(void);
void Test_ResetGlobals(void);
void Test_SetCursor(U32 cur);
void Test_SetTickPerSecond(U32 tps);
void Test_BindTimerArray(struct TagSwTmrCtrl *arr, U32 maxNum);

void Test_InitStats(struct TestStats *st);
void Test_CheckEqU32(const char *name, U32 expect, U32 actual, struct TestStats *st);
void Test_CheckEqU8(const char *name, U8 expect, U8 actual, struct TestStats *st);
void Test_Summary(const char *title, const struct TestStats *st);
void Test_CheckEqPtr(const char *name, const void *expect, const void *actual, struct TestStats *st);

void reset_test_state(void);
void init_task(struct TagTskCb *tsk, uint32_t pid, uint32_t policy, uint32_t timeSlice);

#endif /* TEST_SUPPORT_H */