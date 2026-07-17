/*
 * QSemOS Runtime Stubs for Linux Testing
 * 
 * 提供 QSemOS 内核函数的桩实现，用于在 Linux 环境下进行单元测试。
 * 这些桩函数模拟内核行为，不涉及实际的硬件操作。
 */

#ifndef __QSEMOS_RUNTIME_STUBS_H__
#define __QSEMOS_RUNTIME_STUBS_H__

#include "qsem_types.h"

/* ============================================================
 * Interrupt Lock/Restore Stubs
 * ============================================================ */
STATIC uintptr_t OsIntLock(void) { return 0; }
STATIC void OsIntRestore(uintptr_t intSave) { (void)intSave; }

/* ============================================================
 * Memory Allocation Stubs
 * ============================================================ */
STATIC void *PRT_MemAlloc(uint32_t poolId, uint32_t pt, uint32_t size) {
    (void)poolId; (void)pt;
    return malloc(size);
}

STATIC uint32_t PRT_MemFree(uint32_t poolId, void *ptr) {
    (void)poolId;
    free(ptr);
    return OS_OK;
}

/* ============================================================
 * Task Related Stubs
 * ============================================================ */
STATIC void *OsMemAllocAlign(U32 poolId, U32 pt, U32 size, U32 align) {
    (void)poolId; (void)pt; (void)align;
    return malloc(size);
}

/* Task status bits */
#define TSK_STATUS_UNUSED 0x0001
#define TSK_STATUS_RUNNING 0x0002
#define TSK_STATUS_READY 0x0004
#define TSK_STATUS_PEND 0x0008
#define TSK_STATUS_SUSPEND 0x0010
#define TSK_STATUS_DELAY 0x0020

#define TSK_STATUS_SET(tsk, statBit) ((tsk)->taskStatus |= (statBit))
#define TSK_STATUS_TST(tsk, statBit) (((tsk)->taskStatus & (statBit)) != 0)

/* Task ID validation */
#define CHECK_TSK_PID_OVERFLOW(pid) 0  /* Simplified for testing */
#define TSK_IS_UNUSED(taskCb) (((taskCb)->taskStatus & TSK_STATUS_UNUSED) != 0)

/* Get TCB handle */
#define GET_TCB_HANDLE(pid) ((void*)(uintptr_t)(pid))  /* Simplified */

/* Global task variables (stubs) */
extern U32 g_tskMaxNum;

/* ============================================================
 * Semaphore Related Stubs
 * ============================================================ */
STATIC void OsSemPrioLock(void) { }
STATIC void OsSemPrioUnLock(void) { }

/* ============================================================
 * Scheduler Stubs
 * ============================================================ */
STATIC void OsTskSchedule(void) { }
STATIC void OsSpinLockTaskRq(void *tcb) { (void)tcb; }
STATIC void OsSpinUnlockTaskRq(void *tcb) { (void)tcb; }
STATIC void OsTskReadyAdd(void *tcb) { (void)tcb; }
STATIC void OsTskReadyDel(void *tcb) { (void)tcb; }
STATIC void OsTskLock(void) { }
STATIC void OsTskUnlock(void) { }

/* ============================================================
 * Ring Buffer Stubs
 * ============================================================ */
typedef struct {
    char *fifo;
    U32 size;
    U32 remain;
    U32 startIdx;
    U32 endIdx;
} Ringbuf;

STATIC U32 PRT_RingbufUsedSize(Ringbuf *rb) {
    if (!rb) return 0;
    return rb->size - rb->remain;
}

STATIC U32 PRT_RingbufInit(Ringbuf *rb, void *buf, U32 size) {
    if (!rb || !buf || size == 0) return OS_ERROR;
    rb->fifo = (char *)buf;
    rb->size = size;
    rb->remain = size;
    rb->startIdx = 0;
    rb->endIdx = 0;
    return OS_OK;
}

STATIC U32 PRT_RingbufRead(Ringbuf *rb, char *dest, U32 size) {
    if (!rb || !dest || size == 0) return 0;
    U32 available = PRT_RingbufUsedSize(rb);
    U32 toRead = (size <= available) ? size : available;
    for (U32 i = 0; i < toRead; ++i) {
        dest[i] = rb->fifo[rb->startIdx];
        rb->startIdx = (rb->startIdx + 1) % rb->size;
    }
    rb->remain += toRead;
    return toRead;
}

STATIC U32 PRT_RingbufWrite(Ringbuf *rb, const char *src, U32 size) {
    if (!rb || !src || size == 0) return 0;
    U32 toWrite = (size <= rb->remain) ? size : rb->remain;
    for (U32 i = 0; i < toWrite; ++i) {
        rb->fifo[rb->endIdx] = src[i];
        rb->endIdx = (rb->endIdx + 1) % rb->size;
    }
    rb->remain -= toWrite;
    return toWrite;
}

/* ============================================================
 * List Operations Stubs
 * ============================================================ */
#define OS_LIST_INIT(obj) do { (obj).next = &(obj); (obj).prev = &(obj); } while(0)
#define ListEmpty(l) ((l)->next == (l))
#define ListAdd(n, l) do { (n)->next = (l); (n)->prev = (l)->prev; (l)->prev->next = (n); (l)->prev = (n); } while(0)

typedef struct TagListObject {
    struct TagListObject *next;
    struct TagListObject *prev;
} ListHead;

#define OS_LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define OS_LIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define OS_LIST_FIRST(head) ((head)->next)

#define GET_TCB_PEND(node) ((void*)((char*)(node) - offsetof(struct TagTskCb, pendList)))

/* ============================================================
 * Timer Related Stubs
 * ============================================================ */
#define TIMER_TYPE_INVALID 3
#define TIMER_TYPE_SWTMR 1
#define TIMER_TYPE_HWTMR 0

typedef struct TagFuncsLibTimer {
    void *createTimer;
    void *deleteTimer;
    void *startTimer;
    void *stopTimer;
    void *getRemain;
    void *getOverrun;
} TagFuncsLibTimer;

extern TagFuncsLibTimer g_timerApi[TIMER_TYPE_INVALID];

/* ============================================================
 * Secure memset
 * ============================================================ */
STATIC int memset_s(void *dest, size_t destsz, int ch, size_t count) {
    if (!dest) return (int)((U32)'E' | ((U32)'N' << 8) | ((U32)'O' << 16) | ((U32)'S' << 24));
    if (count > destsz) return (int)((U32)'E' | ((U32)'R' << 8) | ((U32)'N' << 16) | ((U32)'O' << 24));
    memset(dest, ch, count);
    return 0;
}

#endif /* __QSEMOS_RUNTIME_STUBS_H__ */
