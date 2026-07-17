#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "auto_stub.h"
#include "../include/prt_swtmr_internal.h"

/* Timer globals */
extern volatile U64 g_uniTicks;
extern struct TagSwTmrCtrl *g_swtmrCbArray;
extern U32 g_swTmrMaxNum;
extern TagSwTmrSortLinkCtx g_tmrSortLink;
extern struct TagSwTmrCtrl *g_tmrFreeList;
extern struct TickModInfo g_tickModInfo;

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

#endif /* TEST_SUPPORT_H */
