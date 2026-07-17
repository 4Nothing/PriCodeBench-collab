/*
 * QSemOS Check Test Framework - Weak Stubs Implementation
 * 
 * 提供 QSemOS 内核函数的 weak 实现，可被 test.c 中的定义覆盖。
 */

#include "qsem_test_types.h"

/* Perf output functions - weak implementations */
__attribute__((weak)) void OsPerfOutPutFlush(void) { }
__attribute__((weak)) void OsPerfDefaultNotify(void) { }

/* Semaphore create functions - weak implementations */
__attribute__((weak)) U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) {
    (void)count; (void)semType; (void)semMode; if (semHandle) *semHandle = 0; (void)cookie; return OS_OK;
}
__attribute__((weak)) void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, TagSemCb *semCreated) {
    (void)count; (void)semType; (void)semMode; (void)semCreated;
}
__attribute__((weak)) U32 OsSemInitCb(void) { return OS_OK; }
__attribute__((weak)) void OsTimerDelayCount(U64 cycles) { (void)cycles; }

/* Timer functions - weak implementations */
__attribute__((weak)) U32 OsSwTmrStartTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
__attribute__((weak)) U32 OsSwTmrRestartTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
__attribute__((weak)) U32 OsSwTmrQuery(TimerHandle tmrHandle, U32 *remain) { (void)tmrHandle; if (remain) *remain = 0; return OS_OK; }
__attribute__((weak)) U32 OsSwTmrGetOverrun(TimerHandle tmrHandle, U32 *overrun) { (void)tmrHandle; if (overrun) *overrun = 0; return OS_OK; }
__attribute__((weak)) U32 OsSwTmrCreateTimerParaChk(struct TimerCreatePara *para) { (void)para; return OS_OK; }
__attribute__((weak)) U32 OsSwTmrGetRemainTick(struct TagSwTmrCtrl *swtmr) { (void)swtmr; return 0; }

/* Timer group functions - weak implementation */
__attribute__((weak)) U32 OsSwTimerGroupCreate(struct TmrGrpUserCfg *config, TimerGroupId *grpId) { (void)config; if (grpId) *grpId = 0; return OS_OK; }

/* Timer scan functions - weak implementations */
__attribute__((weak)) void OsSwTmrScan(void) { }
__attribute__((weak)) void OsSwTmrStop(TagSwTmrCtrl *swtmr, bool needSchedule) { (void)swtmr; (void)needSchedule; }
__attribute__((weak)) void OsSwTmrDelete(TagSwTmrCtrl *swtmr) { (void)swtmr; }
__attribute__((weak)) void OsSwtmrNearestTicksRefresh(void *tmrSortLink) { (void)tmrSortLink; }

/* Timer IRQ lock functions - weak implementations */
__attribute__((weak)) uintptr_t OsSwtmrIqrSplLock(TagSwTmrCtrl *swtmr) { (void)swtmr; return 0; }
__attribute__((weak)) uintptr_t OsSwtmrIqrSplUnlock(TagSwTmrCtrl *swtmr, uintptr_t intSave) { (void)swtmr; (void)intSave; return 0; }

/* Tickless functions - weak implementations */
__attribute__((weak)) U64 OsSwtmrNearestTickGet(U32 coreId) { (void)coreId; return 0; }
__attribute__((weak)) bool OsSwtmrTargetCheck(U32 coreId) { (void)coreId; return false; }

/* Ring buffer functions - weak implementations */
__attribute__((weak)) U32 OsRingbufReadLoop(Ringbuf *ringbuf, char *buf, U32 size) { (void)ringbuf; (void)buf; (void)size; return 0; }
__attribute__((weak)) U32 OsRingbufWriteLoop(Ringbuf *ringbuf, const char *src, U32 size) { (void)ringbuf; (void)src; (void)size; return 0; }

/* Spinlock functions - weak implementations */
__attribute__((weak)) void OsSplLock(SplLock *lock) { (void)lock; }
__attribute__((weak)) void OsSplUnlock(SplLock *lock) { (void)lock; }

/* RwLock functions - weak implementations */
__attribute__((weak)) U32 OsRwLockCheck(void *rwl) { (void)rwl; return OS_OK; }
__attribute__((weak)) U32 OsRwLockPost(void *rwl, bool *needSched) { (void)rwl; if (needSched) *needSched = false; return OS_OK; }
__attribute__((weak)) U32 OsRwLockPendFindPosSub(struct TagTskCb *task, TagRwLockCb *rwl) { (void)task; (void)rwl; return OS_OK; }
__attribute__((weak)) U32 OsRwLockPendFindPos(struct TagTskCb *task, TagRwLockCb *rwl) { (void)task; (void)rwl; return 0; }
__attribute__((weak)) void OsRwLockPendPre(struct TagTskCb *runTask, struct TagListObject *list, U32 timeout, pthread_rwlock_t *rwl) { (void)runTask; (void)list; (void)timeout; (void)rwl; }
__attribute__((weak)) U32 OsRwLockGetMode(struct TagListObject *readList, struct TagListObject *writeList) { (void)readList; (void)writeList; return RWLOCK_NONE_MODE; }
__attribute__((weak)) U32 OsRwLockPendSchedule(struct TagTskCb *runTask, struct TagListObject *readList, U32 timeout, U32 intSave, pthread_rwlock_t *rwl) { (void)runTask; (void)readList; (void)timeout; (void)intSave; (void)rwl; return OS_OK; }
__attribute__((weak)) bool OsRwlockPriCompare(struct TagTskCb *task, struct TagListObject *writeList) { (void)task; (void)writeList; return false; }

/* Secure memset - must be non-static for linking */
int memset_s(void *dest, size_t destsz, int ch, size_t count) {
    if (!dest) return (int)((U32)'E' | ((U32)'N' << 8) | ((U32)'O' << 16) | ((U32)'S' << 24));
    if (count > destsz) return (int)((U32)'E' | ((U32)'R' << 8) | ((U32)'N' << 16) | ((U32)'O' << 24));
    memset(dest, ch, count);
    return 0;
}

void *memcpy_s(void *dest, size_t destsz, const void *src, size_t count) {
    if (!dest || !src || count > destsz) return NULL;
    memcpy(dest, src, count);
    return dest;
}