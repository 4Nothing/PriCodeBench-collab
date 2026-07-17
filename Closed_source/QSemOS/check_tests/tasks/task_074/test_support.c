#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "auto_stub.h"
#include "../include/prt_swtmr_internal.h"

/*
 Minimal standalone runtime to support unit tests for prt_swtmr_minor.c
 This file provides:
 - Required globals
 - Interrupt/spinlock stubs (using macros from auto_stub.h)
 - A simple timer wheel initialization
 - A minimal OsSwTmrStart() implementation sufficient for our tests
 - Tiny test harness helpers (assert/summary)
*/

volatile U64 g_uniTicks = 0; /* not used in non-SMP tests */
struct TagSwTmrCtrl *g_swtmrCbArray = NULL;
U32 g_swTmrMaxNum = 0;
TagSwTmrSortLinkCtx g_tmrSortLink;
struct TagSwTmrCtrl *g_tmrFreeList = NULL; /* non-SMP free list head */
struct TickModInfo g_tickModInfo;

/* Wheel storage */
static struct TagListObject g_wheel[OS_SWTMR_SORTLINK_LEN];

/* --- Tiny harness --- */
struct TestStats
{
    int total;
    int pass;
};

void Test_InitWheel(void)
{
    g_tmrSortLink.cursor = 0;
    g_tmrSortLink.sortLink = g_wheel;
    for (U32 i = 0; i < OS_SWTMR_SORTLINK_LEN; ++i)
    {
        struct TagListObject *head = &g_wheel[i];
        head->prev = head;
        head->next = head;
    }
}

void Test_ResetGlobals(void)
{
    g_tmrFreeList = NULL;
    g_uniTicks = 0;
    g_tickModInfo.tickPerSecond = 1000;
}

void Test_SetCursor(U32 cur) { g_tmrSortLink.cursor = cur % OS_SWTMR_SORTLINK_LEN; }
void Test_SetTickPerSecond(U32 tps) { g_tickModInfo.tickPerSecond = tps ? tps : 1; }
void Test_BindTimerArray(struct TagSwTmrCtrl *arr, U32 maxNum)
{
    g_swtmrCbArray = arr;
    g_swTmrMaxNum = maxNum;
}

static void list_insert_after(struct TagListObject *head, struct TagSwTmrCtrl *node)
{
    // Insert as first element after head (at the front of the slot list)
    struct TagSwTmrCtrl *first = (struct TagSwTmrCtrl *)(void *)head->next;
    node->prev = (struct TagSwTmrCtrl *)(void *)head;
    node->next = first;
    head->next = (struct TagListObject *)(void *)node;
    first->prev = node;
}

/* Minimal OsSwTmrStart used by APIs under test. For our tests we:
 * - compute slot index from ticks (no SMP/tickless adjustments)
 * - put the timer as the first node in that slot list
 * - set its idxRollNum to ticks so OsSwTmrGetRemain() can compute correctly
 *   for ticks < OS_SWTMR_SORTLINK_LEN. Our tests avoid larger values. */
void OsSwTmrStart(struct TagSwTmrCtrl *swtmr, U32 ticks)
{
    U32 idx = ticks % OS_SWTMR_SORTLINK_LEN;
    swtmr->idxRollNum = ticks; /* simple case; tests keep ticks < wheel length */
    struct TagListObject *head = g_tmrSortLink.sortLink + idx;
    list_insert_after(head, swtmr);
    swtmr->state = OS_TIMER_RUNNING;
}

/* --- Assertion helpers --- */
void Test_InitStats(struct TestStats *st)
{
    st->total = 0;
    st->pass = 0;
}

void Test_CheckEqU32(const char *name, U32 expect, U32 actual, struct TestStats *st)
{
    st->total++;
    int ok = (expect == actual);
    if (ok)
        st->pass++;
    printf("[%-24s] expect=%u, actual=%u => %s\n", name, expect, actual, ok ? "PASS" : "FAIL");
}

void Test_CheckEqU8(const char *name, U8 expect, U8 actual, struct TestStats *st)
{
    st->total++;
    int ok = (expect == actual);
    if (ok)
        st->pass++;
    printf("[%-24s] expect=%u, actual=%u => %s\n", name, (unsigned)expect, (unsigned)actual, ok ? "PASS" : "FAIL");
}

void Test_Summary(const char *title, const struct TestStats *st)
{
    double rate = st->total ? (100.0 * (double)st->pass / (double)st->total) : 0.0;
    printf("\n%s Pass-Rate: %.2f%% (%d/%d)\n\n", title, rate, st->pass, st->total);
}

void Test_CheckEqPtr(const char *name, const void *expect, const void *actual, struct TestStats *st)
{
    st->total++;
    int ok = (expect == actual);
    if (ok)
        st->pass++;
    printf("[%-24s] expect=%p, actual=%p => %s\n", name, expect, actual, ok ? "PASS" : "FAIL");
}