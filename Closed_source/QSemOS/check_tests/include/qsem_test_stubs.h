/*
 * QSemOS Check Test Framework - Comprehensive Stubs
 * 
 * 提供 QSemOS 内核函数的桩实现，支持所有 240 个测试任务。
 */

#ifndef __QSEM_TEST_STUBS_H__
#define __QSEM_TEST_STUBS_H__

#include "qsem_test_types.h"

/* Common utility macros */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Enable POSIX.199309 for SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

/* POSIX signal constants for QSemOS signal functions */
#include <signal.h>
#include <stdarg.h>

/* Fallback definitions if not provided by signal.h in strict c99 mode */
#ifndef SIG_BLOCK
#define SIG_BLOCK 0
#endif
#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK 1
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK 2
#endif

/* Override OS_OPTION_SMP based on what test.c defined before including types.h */
/* Save the value that was defined before types.h, or default to 0 */
#ifdef OS_OPTION_SMP_WAS_DEFINED
#undef OS_OPTION_SMP
#define OS_OPTION_SMP OS_OPTION_SMP_WAS_DEFINED
#else
#ifndef OS_OPTION_SMP
#define OS_OPTION_SMP 0
#endif
#endif

/* ============================================================
 * Forward declarations
 * ============================================================ */
struct TagListObject;
struct TagTskCb;

/* ============================================================
 * List Operations Stubs
 * ============================================================ */
#define OS_LIST_INIT(head) do { (head)->prev = (head); (head)->next = (head); } while(0)
#define OS_LIST_INIT_PTR(ptr) do { (ptr)->next = (ptr); (ptr)->prev = (ptr); } while(0)
#define ListEmpty(l) ((l)->next == (l))
#define ListAdd(n, l) do { (n)->next = (l); (n)->prev = (l)->prev; (l)->prev->next = (n); (l)->prev = (n); } while(0)
#define ListDel(n) do { (n)->prev->next = (n)->next; (n)->next->prev = (n)->prev; } while(0)
#define ListTailAdd(n, l) ListAdd(n, l)
#define OS_LIST_FIRST(head) ((head)->next)
#define OS_LIST_LAST(head) ((head)->prev)
#define LIST_LAST(head) ((head)->prev)
#define ListDelete(node) ListDel(node)
#define LIST_FOR_EACH(pos, head, type, member) \
    for (pos = (type *)((char *)(head)->next - offsetof(type, member)); \
         &pos->member != (head); \
         pos = (type *)((char *)pos->member.next - offsetof(type, member)))
/* LIST_FOR_EACH_SAFE with 4 args: pos, head, type, member (n is omitted, not used in these tests) */
#define LIST_FOR_EACH_SAFE(pos, head, type, member) \
    for (pos = (type *)((char *)(head)->next - offsetof(type, member)); \
         &pos->member != (head); \
         pos = (type *)((char *)pos->member.next - offsetof(type, member)))

/* List component macros */
#define OFFSET_OF_FIELD(type, field) ((uintptr_t)((uintptr_t)(&((type *)0x10)->field) - (uintptr_t)0x10))
#define COMPLEX_OF(ptr, type, field) ((type *)((uintptr_t)(ptr) - OFFSET_OF_FIELD(type, field)))
#define LIST_COMPONENT(ptrOfList, typeOfList, fieldOfList) COMPLEX_OF(ptrOfList, typeOfList, fieldOfList)

/* ============================================================
 * Global Variables (stubs)
 * ============================================================ */

/* Boolean constants */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Signal structures - full definition */
struct sigInfoNode {
    signalInfo siginfo;
    struct TagListObject siglist;
};

/* Signal functions */
extern bool sigValid(int signo);
#ifndef OS_SIG_VALID_DEFINED
__attribute__((weak)) bool sigValid(int signo) { (void)signo; return true; }
#endif
#ifndef OS_SIG_WAIT_PROC_PENDING_SIGNAL_DEFINED
extern U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info);
__attribute__((weak)) U32 OsSigWaitProcPendingSignal(struct TagTskCb *runTsk, const signalSet *set, signalInfo *info) { (void)runTsk; (void)set; (void)info; return OS_OK; }
#endif
extern void OsSignalProcKill(TskHandle taskPid, int signo);

/* Signal handling functions */
#ifndef OS_HANDLE_ONE_SIGNAL_DEFINED
extern void OsHandleOneSignal(struct TagTskCb *runTsk, int signum);
__attribute__((weak)) void OsHandleOneSignal(struct TagTskCb *runTsk, int signum) { (void)runTsk; (void)signum; }
#endif

/* Signal timeout set function */
#ifndef OS_SIGNAL_TIMEOUT_SET_DEFINED
extern void OsSignalTimeOutSet(struct TagTskCb *runTsk, U32 timeOutTick);
__attribute__((weak)) void OsSignalTimeOutSet(struct TagTskCb *runTsk, U32 timeOutTick) { (void)runTsk; (void)timeOutTick; }
#endif

/* Signal unblock function */
#ifndef OS_HANDLE_UNBLOCK_SIGNAL_DEFINED
extern void OsHandleUnBlockSignal(struct TagTskCb *runTsk);
__attribute__((weak)) void OsHandleUnBlockSignal(struct TagTskCb *runTsk) { (void)runTsk; }
#endif

/* Signal wait schedule function */
#ifndef OS_SIGNAL_WAIT_SCHE_DEFINED
extern U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick);
__attribute__((weak)) U32 OsSignalWaitSche(struct TagTskCb *runTsk, const signalSet *set, U32 timeOutTick) { (void)runTsk; (void)set; (void)timeOutTick; return OS_OK; }
#endif

/* Signal wait reschedule function */
#ifndef OS_SIGNAL_WAIT_RESCHE_DEFINED
extern U32 OsSignalWaitReSche(struct TagTskCb *runTsk, U32 taskStatus);
__attribute__((weak)) U32 OsSignalWaitReSche(struct TagTskCb *runTsk, U32 taskStatus) { (void)runTsk; (void)taskStatus; return OS_OK; }
#endif

/* Add signal pending flag function */
#ifndef OS_ADD_SIGNAL_PENDING_FLAG_DEFINED
extern U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info);
__attribute__((weak)) U32 OsAddSignalPendingFlag(struct TagTskCb *taskCb, signalInfo *info) { (void)taskCb; (void)info; return OS_OK; }
#endif

/* Signal entry function */
#ifndef OS_SIGNAL_ENTRY_DEFINED
extern void OsSignalEntry(TskHandle taskId);
__attribute__((weak)) void OsSignalEntry(TskHandle taskId) { (void)taskId; }
#endif

/* Task status flags */
#ifndef OS_TSK_HOLD_SIGNAL
#define OS_TSK_HOLD_SIGNAL 0x1000U
#endif

/* Task context load function */
extern void OsTskContextLoad(uintptr_t task);
__attribute__((weak)) void OsTskContextLoad(uintptr_t task) { (void)task; }

/* Task context init function */
#ifndef OS_TSK_CONTEXT_INIT_DEFINED
extern void *OsTskContextInit(TskHandle taskId, U32 stackSize, uintptr_t *topOfStack, uintptr_t entry);
__attribute__((weak)) void *OsTskContextInit(TskHandle taskId, U32 stackSize, uintptr_t *topOfStack, uintptr_t entry) { (void)taskId; (void)stackSize; (void)topOfStack; (void)entry; return NULL; }
#endif

/* System time hook */
typedef U64 (*SysTimeHook)(void);
SysTimeHook g_sysTimeHook = NULL;

/* Software timer related */
struct TagSwTmrCtrl;
typedef struct TagSwTmrCtrl {
    struct TagSwTmrCtrl *next;
    struct TagSwTmrCtrl *prev;
    U8 state;
    U32 timerId;
    U32 swtmrIndex;
    U32 interval;
    U32 idxRollNum;
    U32 overrun;
    U32 mode;
    U32 coreID;
    void (*handler)(void);
    void (*callback)(void);
    void *arg;
    void *arg1;
    void *arg2;
    void *arg3;
    void *arg4;
    U64 expectedTick;
} TagSwTmrCtrl;

/* Timer free list structure */
typedef struct {
    TagSwTmrCtrl *freeList;
    SplLock spinLock;
} SwTmrFreeList;

/* For non-SMP mode, g_tmrFreeList is a pointer; for SMP mode, it's a struct */
#if OS_OPTION_SMP == 0
TagSwTmrCtrl *g_tmrFreeList = NULL;
#define g_tmrFreeListFreeList g_tmrFreeList
#else
SwTmrFreeList g_tmrFreeList = {{NULL}, 0};
#define g_tmrFreeListFreeList g_tmrFreeList.freeList
#endif

#define OS_SWTMR_INDEX_2_ID(index) ((TimerHandle)(index))
#define OS_SWTMR_ID_2_INDEX(handle) ((U32)(handle))
#define OS_TIMER_GET_INDEX(handle) ((U32)(handle))

/* Timer roll number and sort index macros */
#define UWROLLNUM(num) ((num) & 0x03ffffffU)
#define UWSORTINDEX(num) ((num) >> 26)
#define UWROLLNUMADD(num1, num2) ((num1) = (((num1) & 0xfc000000U) | (UWROLLNUM(num1) + UWROLLNUM(num2))))
#define UWROLLNUMSUB(num1, num2) ((num1) = (((num1) & 0xfc000000U) | (UWROLLNUM(num1) - UWROLLNUM(num2))))
#define UWROLLNUMDEC(num) ((num) = ((num) - 1))
#define EVALUATE_H(num, value) ((num) = (((num) & 0x03ffffffU) | ((value) << 26)))
#define EVALUATE_L(num, value) ((num) = (((num) & 0xfc000000U) | (value)))
#define SWTMR_CREATE_DEL_UNLOCK() do {} while(0)
#define SWTMR_CREATE_DEL_LOCK() do {} while(0)
#define OS_TIMER_CREATED 1

/* Timer lock/unlock functions - defined in test.c (task_073, task_074), weak impl for callers */
#ifndef OS_SWTMR_IQR_SPL_LOCK_DEFINED
__attribute__((weak)) uintptr_t OsSwtmrIqrSplLock(TagSwTmrCtrl *swtmr) { (void)swtmr; return 0; }
#endif
#ifndef OS_SWTMR_IQR_SPL_UNLOCK_DEFINED
__attribute__((weak)) uintptr_t OsSwtmrIqrSplUnlock(TagSwTmrCtrl *swtmr, uintptr_t intSave) { (void)swtmr; (void)intSave; return 0; }
#endif
static inline U32 OsGetCoreID(void) { return 0; }

/* Timer scan function - weak stub implementation (OsSwTmrStop defined in test.c task_071, weak for other callers) */
#ifndef OS_SWTMR_SCAN_DEFINED
__attribute__((weak)) void OsSwTmrScan(void) { }
#endif
#ifndef OS_SWTMR_START_DEFINED
extern U32 OsSwTmrStart(struct TagSwTmrCtrl *swtmr, U32 interval);
__attribute__((weak)) U32 OsSwTmrStart(struct TagSwTmrCtrl *swtmr, U32 interval) { (void)swtmr; (void)interval; return OS_OK; }
#endif
#ifndef OS_SWTMR_STOP_DEFINED
__attribute__((weak)) void OsSwTmrStop(TagSwTmrCtrl *swtmr, bool needSchedule) { (void)swtmr; (void)needSchedule; }
#endif
#ifndef OS_SWTMR_DELETE_DEFINED
__attribute__((weak)) void OsSwTmrDelete(TagSwTmrCtrl *swtmr) { (void)swtmr; }
#endif
#ifndef OS_SWTMR_NEAREST_TICKS_REFRESH_DEFINED
__attribute__((weak)) void OsSwtmrNearestTicksRefresh(void *tmrSortLink) { (void)tmrSortLink; }
#endif

/* Performance related */
typedef void (*PERF_BUF_NOTIFY_HOOK)(void);
typedef void (*PERF_BUF_FLUSH_HOOK)(void *addr, U32 size);
PERF_BUF_NOTIFY_HOOK g_perfBufNotifyHook = NULL;
PERF_BUF_FLUSH_HOOK g_perfBufFlushHook = NULL;

/* Ring buffer related - use definition from qsem_test_types.h */
PerfOutputCB g_perfOutputCb;
PerfCB g_perfCb;

/* Perf related */
#define PERF_MAX_EVENT 8
/* OsPerfPmuGet - test file provides implementation */
#ifndef OS_PERF_PMU_GET_DEFINED
#define OS_PERF_PMU_GET_DEFINED
extern Pmu *OsPerfPmuGet(U32 type);
#endif
#ifndef OS_PERF_CONFIG_DEFINED
#define OS_PERF_CONFIG_DEFINED
extern U32 OsPerfConfig(PerfEventConfig *config);
#endif

/* Task related */
typedef struct TagTskCb {
    U32 taskStatus;
    TskPrior priority;
    TskPrior origPriority;
    TskHandle taskPid;
    struct TagListObject pendList;
    struct TagListObject sigInfoList;
    struct TagListObject semBList;
    U32 sigPending;
    struct TagSemCb *taskPend;
    /* Additional members needed by tests */
    char name[OS_TSK_NAME_LEN];
    void *stackPointer;
    void *topOfStack;
    U32 stackSize;
    U32 args[4];
    void (*taskEntry)(U32, U32, U32, U32);
    locale_t locale;
    U32 coreID;
    bool isOnRq;
    U32 expirationTick;
    struct TagListObject timerList;
    U32 policy;
    U32 timeSlice;
    U32 lastErr;
    /* Signal related members */
    SigSet sigMask;
    SigSet sigWaitMask;
    int holdSignal;
    void *oldStackPointer;
#define PRT_SIGNAL_MAX OS_SIGNAL_MAX
    _sa_handler sigVectors[OS_SIGNAL_MAX];
    /* Perf-related fields needed by prt_perf.c */
    uintptr_t pc;
    uintptr_t fp;
} TagTskCb;
U32 g_tskMaxNum = 10;
#define IDLE_TASK_ID 0

/* Task sorted delay list structure */
typedef struct TagOsTskSortedDelayList {
    struct TagListObject *sortLink;
    struct TagListObject *tskList;
    U32 cursor;
} TagOsTskSortedDelayList;

/* Running task stub */
TagTskCb *RUNNING_TASK = NULL;

/* Semaphore related */
typedef struct TagSemCb {
    U32 semStat;
    U32 semCount;
    U32 semType;
    U32 semMode;
    U32 semId;
    TskHandle semOwner;
    struct TagListObject semBList;
    struct TagListObject semList;
} TagSemCb;
#define GET_SEM(handle) ((TagSemCb*)(uintptr_t)(handle))
#define GET_SEM_TYPE(type) ((type) & 0x0F)
#define GET_MUTEX_TYPE(type) ((type) >> 4)
#define SEM_TYPE_BIN 1
#define SEM_MUTEX_TYPE_RECUR 2
#define GET_SEM_LIST(node) ((TagSemCb*)((char*)(node) - offsetof(TagSemCb, semList)))

/* Global semaphore list */
struct TagListObject g_unusedSemList;
U32 g_swTmrMaxNum = 0;
U32 g_maxSem = 0;
TagSwTmrCtrl *g_swtmrCbArray = NULL;
void (*g_swtmrScanHook)(void) = NULL;
SplLock g_semPrioLock;
U32 g_systemClock = 100000000;

/* Unified tick counter */
U64 g_uniTicks = 0;
U64 g_ticksOffset = 0;

/* Timer sort link structure for non-SMP */
typedef struct {
    struct TagListObject *sortLink;
    U32 cursor;
} SwTmrSortLink;
SwTmrSortLink g_tmrSortLink = {NULL, 0};

/* Timer function pointer types */
typedef U32 (*TimerCreateFunc)(struct TimerCreatePara *, TimerHandle *);
typedef U32 (*TimerDeleteFunc)(TimerHandle);
typedef U32 (*TimerStartFunc)(TimerHandle);
typedef U32 (*TimerStopFunc)(TimerHandle);
typedef U32 (*TimerRestartFunc)(TimerHandle);
typedef U32 (*TimerGetRemainFunc)(TimerHandle);
typedef U32 (*TimerGetOverrunFunc)(TimerHandle, U32 *);
typedef U32 (*TimerQueryFunc)(TimerHandle, U32 *);

typedef struct TagFuncsLibTimer {
    TimerCreateFunc createTimer;
    TimerDeleteFunc deleteTimer;
    TimerStartFunc startTimer;
    TimerStopFunc stopTimer;
    TimerRestartFunc restartTimer;
    TimerGetRemainFunc getRemain;
    TimerGetOverrunFunc getOverrun;
    TimerQueryFunc timerQuery;
} TagFuncsLibTimer;
TagFuncsLibTimer g_timerApi[TIMER_TYPE_INVALID];

typedef struct {
    U32 tickPerSecond;
} TickModInfo;
TickModInfo g_tickModInfo = {100};

/* HWI related */
HwiNum g_defHandlerHwiNum = 0;

/* HWI initialization functions */
#ifndef OS_HWI_SPINLOCK_INIT_DEFINED
#define OS_HWI_SPINLOCK_INIT() do {} while(0)
#endif
#ifndef OS_HWI_DESC_INIT_ALL_DEFINED
extern void OsHwiDescInitAll(void);
__attribute__((weak)) void OsHwiDescInitAll(void) {}
#endif
#ifndef OS_HWI_DESC_FIELD_INIT_DEFINED
#define OS_HWI_DESC_FIELD_INIT_DEFINED
extern void OsHwiDescFieldInit(U32 irqNum);
__attribute__((weak)) void OsHwiDescFieldInit(U32 irqNum) { (void)irqNum; }
#endif
#ifndef OS_HWI_GIC_INIT_DEFINED
extern void OsHwiGICInit(void);
__attribute__((weak)) void OsHwiGICInit(void) {}
#endif

/* HWI mode and number check macros */
#ifndef OS_HWI_NUM_CHECK_DEFINED
#define OS_HWI_NUM_CHECK_DEFINED
#define OS_HWI_NUM_CHECK(hwiNum) ((hwiNum) >= OS_HWI_MAX_NUM)
#endif

#ifndef OS_HWI_MODE_INV_DEFINED
#define OS_HWI_MODE_INV_DEFINED
#define OS_HWI_MODE_INV(hwiNum) (0)
#endif

#ifndef PRT_HWI_DISABLE_DEFINED
#define PRT_HWI_DISABLE_DEFINED
extern void PRT_HwiDisable(U32 hwiNum);
__attribute__((weak)) void PRT_HwiDisable(U32 hwiNum) { (void)hwiNum; }
#endif

#ifndef OS_HWI_GET_HWINUM_DEFINED
#define OS_HWI_GET_HWINUM_DEFINED
#define OS_HWI_GET_HWINUM(archHwi) ((HwiHandle)(archHwi))
#endif

#ifndef OS_HWI2IRQ_DEFINED
#define OS_HWI2IRQ_DEFINED
#define OS_HWI2IRQ(hwiNum) ((U32)(hwiNum))
#endif

#ifndef OS_MHOOK_ACTIVATE_PARA0_DEFINED
#define OS_MHOOK_ACTIVATE_PARA0_DEFINED
#define OS_MHOOK_ACTIVATE_PARA0(hook) do {} while(0)
#endif
#ifndef OS_MHOOK_ACTIVATE_PARA1_DEFINED
#define OS_MHOOK_ACTIVATE_PARA1_DEFINED
#define OS_MHOOK_ACTIVATE_PARA1(hook, para1) do {} while(0)
#endif
#ifndef OS_SHOOK_ACTIVATE_PARA0_DEFINED
#define OS_SHOOK_ACTIVATE_PARA0_DEFINED
#define OS_SHOOK_ACTIVATE_PARA0(hook) do {} while(0)
#endif

/* OS_HOOK_HWI_ENTRY and OS_HOOK_HWI_EXIT are defined in types.h */

#ifndef OS_HWI_HANDLE_ACTIVE_DEFINED
#define OS_HWI_HANDLE_ACTIVE_DEFINED
extern void OsHwiHandleActive(U32 irqNum);
__attribute__((weak)) void OsHwiHandleActive(U32 irqNum) { (void)irqNum; }
#endif

#ifndef OS_HWI_IRQ_LOCK_DEFINED
#define OS_HWI_IRQ_LOCK_DEFINED
#define OS_HWI_IRQ_LOCK(lock) do {} while(0)
#endif

#ifndef OS_HWI_IRQ_UNLOCK_DEFINED
#define OS_HWI_IRQ_UNLOCK_DEFINED
#define OS_HWI_IRQ_UNLOCK(lock) do {} while(0)
#endif

#ifndef OS_HWI_CONNECT_HANDLE_DEFINED
#define OS_HWI_CONNECT_HANDLE_DEFINED
extern U32 OsHwiConnectHandle(U32 irqNum, void *handler, U32 arg);
__attribute__((weak)) U32 OsHwiConnectHandle(U32 irqNum, void *handler, U32 arg) {
    (void)irqNum; (void)handler; (void)arg;
    return OS_OK;
}
#endif

#ifndef OS_HWI_DELETE_FORM_RESUME_DEFINED
#define OS_HWI_DELETE_FORM_RESUME_DEFINED
extern U32 OsHwiDeleteFormResume(U32 irqNum);
__attribute__((weak)) U32 OsHwiDeleteFormResume(U32 irqNum) {
    (void)irqNum;
    return OS_OK;
}
#endif

#ifndef OS_HWI_SET_ATTR_PARA_CHECK_DEFINED
#define OS_HWI_SET_ATTR_PARA_CHECK_DEFINED
extern U32 OsHwiSetAttrParaCheck(HwiHandle hwiNum, HwiPrior hwiPrio, HwiMode mode);
__attribute__((weak)) U32 OsHwiSetAttrParaCheck(HwiHandle hwiNum, HwiPrior hwiPrio, HwiMode mode) {
    (void)hwiNum; (void)hwiPrio; (void)mode;
    return OS_OK;
}
#endif

#ifndef OS_HWI_SET_ATTR_CONFLICT_ERR_CHECK_DEFINED
#define OS_HWI_SET_ATTR_CONFLICT_ERR_CHECK_DEFINED
extern U32 OsHwiSetAttrConflictErrCheck(U32 irqNum, HwiPrior hwiPrio, HwiMode mode);
__attribute__((weak)) U32 OsHwiSetAttrConflictErrCheck(U32 irqNum, HwiPrior hwiPrio, HwiMode mode) {
    (void)irqNum; (void)hwiPrio; (void)mode;
    return OS_OK;
}
#endif

#ifndef OS_HWI_PRIORITY_GET_DEFINED
#define OS_HWI_PRIORITY_GET_DEFINED
extern HwiPrior OsHwiPriorityGet(HwiHandle hwiNum);
__attribute__((weak)) HwiPrior OsHwiPriorityGet(HwiHandle hwiNum) {
    (void)hwiNum;
    return 0;
}
#endif

#ifndef OS_HWI_PRIORITY_SET_DEFINED
#define OS_HWI_PRIORITY_SET_DEFINED
extern void OsHwiPrioritySet(HwiHandle hwiNum, HwiPrior hwiPrio);
__attribute__((weak)) void OsHwiPrioritySet(HwiHandle hwiNum, HwiPrior hwiPrio) {
    (void)hwiNum; (void)hwiPrio;
}
#endif

#ifndef OS_HWI_ATTR_SET_DEFINED
#define OS_HWI_ATTR_SET_DEFINED
extern void OsHwiAttrSet(U32 irqNum, HwiPrior hwiPrio, HwiMode mode);
__attribute__((weak)) void OsHwiAttrSet(U32 irqNum, HwiPrior hwiPrio, HwiMode mode) {
    (void)irqNum; (void)hwiPrio; (void)mode;
}
#endif

/* ============================================================
 * Interrupt Lock/Restore Stubs
 * ============================================================ */
static inline uintptr_t OsIntLock(void) { return 0; }
static inline void OsIntRestore(uintptr_t intSave) { (void)intSave; }

/* Semaphore IRQ lock macros */
#define SEM_INIT_IRQ_LOCK(intSave) intSave = OsIntLock()
#define SEM_INIT_IRQ_UNLOCK(intSave) OsIntRestore(intSave)

/* ============================================================
 * Memory Allocation Stubs
 * ============================================================ */
static inline void *PRT_MemAlloc(U32 poolId, U32 pt, U32 size) {
    (void)poolId; (void)pt;
    return malloc(size);
}

static inline U32 PRT_MemFree(U32 poolId, void *ptr) {
    (void)poolId;
    free(ptr);
    return OS_OK;
}

static inline void *OsMemAllocAlign(U32 poolId, U32 pt, U32 size, U32 align) {
    (void)poolId; (void)pt; (void)align;
    return malloc(size);
}
__attribute__((weak)) void *PRT_MemAllocAlign(U32 poolId, U32 pt, U32 size, U32 align) {
    (void)poolId; (void)pt; (void)align;
    return malloc(size);
}

/* ============================================================
 * Task Related Stubs
 * ============================================================ */
#define TSK_STATUS_UNUSED 0x0001
#define TSK_STATUS_RUNNING 0x0002
#define TSK_STATUS_READY 0x0004
#define TSK_STATUS_PEND 0x0008
#define OS_TSK_WAIT_SIGNAL 0x0040U
#define OS_TSK_DELAY 0x0020U
#define OS_TSK_SIG_PAUSE 0x0080U
#define OS_TSK_DELAY_INTERRUPTIBLE 0x0400U

#define OS_TASK_LOCK_DATA   g_uniTaskLock

#define TSK_STATUS_SET(tsk, statBit) ((tsk)->taskStatus |= (statBit))
#define TSK_STATUS_TST(tsk, statBit) (((tsk)->taskStatus & (statBit)) != 0)
#define TSK_STATUS_CLR(tsk, statBit) ((tsk)->taskStatus &= ~(statBit))
#define TSK_STATUS_CLEAR(tsk, statBit) ((tsk)->taskStatus &= ~(statBit))
#define TSK_IS_UNUSED(taskCb) (((taskCb)->taskStatus & TSK_STATUS_UNUSED) != 0)
#define OS_TSK_DELAY_LOCKED_DETACH(tsk) ListDelete(&(tsk)->timerList)
#define CHECK_TSK_PID_OVERFLOW(pid) 0
#define GET_TCB_HANDLE(pid) ((void*)(uintptr_t)(pid))
#define GET_TCB_PEND(node) ((struct TagTskCb*)((char*)(node) - offsetof(struct TagTskCb, pendList)))

/* ============================================================
 * Semaphore Related Stubs
 * ============================================================ */
static inline void OsSemPrioLock(void) { }
static inline void OsSemPrioUnLock(void) { }
#ifndef OS_SEM_PRIO_BLIST_LOCK_DEFINED
__attribute__((weak)) U32 OsSemPrioBListLock(struct TagTskCb *taskCb) { (void)taskCb; return 0; }
#endif
#ifndef OS_SEM_PRIO_BLIST_UNLOCK_DEFINED
__attribute__((weak)) void OsSemPrioBListUnLock(U32 flag) { (void)flag; }
#endif

/* ============================================================
 * Scheduler Stubs
 * ============================================================ */
static inline void OsTskSchedule(void) { }
static inline void OsSpinLockTaskRq(void *tcb) { (void)tcb; }
static inline void OsSpinUnlockTaskRq(void *tcb) { (void)tcb; }
#ifndef OS_TSK_READY_ADD_DEFINED
static inline void OsTskReadyAdd(void *tcb) { (void)tcb; }
#endif
static inline void OsTskReadyAddBgd(void *tcb) { (void)tcb; }
#ifndef OS_TSK_READY_ADD_ONLY_DEFINED
static inline void OsTskReadyAddOnly(struct TagTskCb *task) { (void)task; }
#endif
static inline void OsReschedTask(struct TagTskCb *task) { (void)task; }
static inline void OsReschedTaskNoWakeIpc(struct TagTskCb *task) { (void)task; }
#ifndef OS_TSK_READY_DEL_DEFINED
static inline void OsTskReadyDel(void *tcb) { (void)tcb; }
#endif
#define GET_RUNQ(coreId) ((struct TagOsRunQue *)NULL)
#define UNLIKELY(x) (x)
#define LIKELY(x) (x)
#define LOAD_FENCE() ((void)0)
#define STORE_FENCE() ((void)0)
#define M_FENCE() do { (void)0; } while(0);
static inline void OsEnqueueTask(struct TagOsRunQue *rq, struct TagTskCb *task, U32 unused) { (void)rq; (void)task; (void)unused; }
static inline void OsDequeueTask(struct TagOsRunQue *rq, struct TagTskCb *task, U32 unused) { (void)rq; (void)task; (void)unused; }
static inline void OsIncNrRunning(struct TagOsRunQue *rq) { (void)rq; }
static inline void OsDecNrRunning(struct TagOsRunQue *rq) { (void)rq; }
static inline void OsTskLock(void) { }
static inline void OsTskUnlock(void) { }
#ifndef OS_TASK_IRQ_UNLOCK_DEFINED
static inline void OsTaskIrqUnlock(uintptr_t intSave) { (void)intSave; }
#endif
#ifndef OS_TASK_PRIORITY_SET_CHECK_DEFINED
__attribute__((weak)) U32 OsTaskPrioritySetCheck(TskHandle taskPid, TskPrior taskPrio) { (void)taskPid; (void)taskPrio; return OS_OK; }
#endif

/* Task timer function */
#ifndef OS_TSK_TIMER_ADD_DEFINED
static inline void OsTskTimerAdd(void *tcb, U32 timeout) { (void)tcb; (void)timeout; }
#endif
#define OS_SET_DLYBASE_AND_TSK_CORE(dlyBase, taskCb) do {} while(0)
#define CPU_OVERTIME_SORT_LIST_LOCK(dlyBase) do {} while(0)
#define CPU_OVERTIME_SORT_LIST_UNLOCK(dlyBase) do {} while(0)
static inline void OsTskDlyNearestTicksRefresh(void *dlyBase) { (void)dlyBase; }

/* Semaphore create functions - may be oracle */
#ifndef OS_SEM_CREATE_DEFINED
extern U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie);  /* defined in test.c */
__attribute__((weak)) U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) { (void)count; (void)semType; (void)semMode; if (semHandle) *semHandle = 0; (void)cookie; return OS_OK; }
#endif
#ifndef OS_SEM_CREATE_CB_DEFINED
__attribute__((weak)) U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) { (void)count; (void)semType; (void)semMode; if (semHandle) *semHandle = 0; (void)cookie; return OS_OK; }
#endif
extern void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, TagSemCb *semCreated);
#ifndef OS_SEM_CREATE_CB_INIT_DEFINED
__attribute__((weak)) void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, TagSemCb *semCreated) { (void)count; (void)semType; (void)semMode; (void)semCreated; }
#endif
extern U32 OsSemInitCb(void);
__attribute__((weak)) U32 OsSemInitCb(void) { return OS_OK; }
#ifndef OS_TIMER_DELAY_COUNT_DEFINED
extern void OsTimerDelayCount(U64 cycles);
__attribute__((weak)) void OsTimerDelayCount(U64 cycles) { (void)cycles; }
#endif

/* Timer functions - may be provided by oracles (extern declarations only) */
#ifndef OS_SWTMR_RES_INIT_DEFINED
extern U32 OsSwTmrResInit(void);  /* defined in test.c */
__attribute__((weak)) U32 OsSwTmrResInit(void) { return OS_OK; }
#endif
#ifndef OS_SWTMR_START_TIMER_DEFINED
extern U32 OsSwTmrStartTimer(TimerHandle tmrHandle);
__attribute__((weak)) U32 OsSwTmrStartTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
#endif
#ifndef OS_SWTMR_STOP_TIMER_DEFINED
extern U32 OsSwTmrStopTimer(TimerHandle tmrHandle);  /* defined in test.c task_072 */
__attribute__((weak)) U32 OsSwTmrStopTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
#endif
#ifndef OS_SWTMR_DELETE_TIMER_DEFINED
extern U32 OsSwTmrDeleteTimer(TimerHandle tmrHandle);  /* defined in test.c */
__attribute__((weak)) U32 OsSwTmrDeleteTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
#endif
#ifndef OS_SWTMR_RESTART_TIMER_DEFINED
extern U32 OsSwTmrRestartTimer(TimerHandle tmrHandle);
__attribute__((weak)) U32 OsSwTmrRestartTimer(TimerHandle tmrHandle) { (void)tmrHandle; return OS_OK; }
#endif
#ifndef OS_SWTMR_QUERY_DEFINED
extern U32 OsSwTmrQuery(TimerHandle tmrHandle, U32 *remain);
__attribute__((weak)) U32 OsSwTmrQuery(TimerHandle tmrHandle, U32 *remain) { (void)tmrHandle; if (remain) *remain = 0; return OS_OK; }
#endif
#ifndef OS_SWTMR_GET_OVERRUN_DEFINED
extern U32 OsSwTmrGetOverrun(TimerHandle tmrHandle, U32 *overrun);
__attribute__((weak)) U32 OsSwTmrGetOverrun(TimerHandle tmrHandle, U32 *overrun) { (void)tmrHandle; if (overrun) *overrun = 0; return OS_OK; }
#endif
#ifndef OS_SWTMR_CREATE_TIMER_DEFINED
extern U32 OsSwTmrCreateTimer(struct TimerCreatePara *para, TimerHandle *tmrHandle);  /* defined in test.c */
__attribute__((weak)) U32 OsSwTmrCreateTimer(struct TimerCreatePara *para, TimerHandle *tmrHandle) { (void)para; if (tmrHandle) *tmrHandle = 0; return OS_OK; }
#endif
#ifndef OS_SWTMR_CREATE_TIMER_PARA_CHK_DEFINED
__attribute__((weak)) U32 OsSwTmrCreateTimerParaChk(struct TimerCreatePara *para) { (void)para; return OS_OK; }
#endif
extern void OsSwTmrCreateTimerCbInit(struct TimerCreatePara *para, TagSwTmrCtrl *swtmr, U32 interval);  /* defined in test.c */
#ifndef OS_SWTMR_CREATE_TIMER_CB_INIT_DEFINED
__attribute__((weak)) void OsSwTmrCreateTimerCbInit(struct TimerCreatePara *para, TagSwTmrCtrl *swtmr, U32 interval) { (void)para; (void)swtmr; (void)interval; }
#endif
#ifndef OS_SWTMR_GET_REMAIN_DEFINED
extern U32 OsSwTmrGetRemain(struct TagSwTmrCtrl *swtmr);  /* oracle in task_066 */
__attribute__((weak)) U32 OsSwTmrGetRemain(struct TagSwTmrCtrl *swtmr) { (void)swtmr; return 0; }
#endif
#ifndef OS_SWTMR_GET_REMAIN_TICK_DEFINED
__attribute__((weak)) U32 OsSwTmrGetRemainTick(struct TagSwTmrCtrl *swtmr) { (void)swtmr; return 0; }
#endif
/* Timer group functions - oracle implementation in test.c if available */
#ifndef OS_SW_TIMER_GROUP_CREATE_DEFINED
__attribute__((weak)) U32 OsSwTimerGroupCreate(struct TmrGrpUserCfg *config, TimerGroupId *grpId) { (void)config; if (grpId) *grpId = 0; return OS_OK; }
#endif

/* Timer group ID */
TimerGroupId g_tickSwTmrGroupId = 0;

/* Core count */
U32 g_maxNumOfCores = 1;

/* Timer sort link attribute array for SMP */
SwTmrSortLinkAttr g_tmrSortLinkAttr[OS_MAX_CORE_NUM] = {{0}};

/* SWTMR sort link macro */
#define CPU_SWTMR_SORT_LINK(idx) (&g_tmrSortLinkAttr[idx])

/* SWTMR control block init function */
#ifndef OS_SWTMR_CTRL_INIT_DEFINED
__attribute__((weak)) void OsSwTmrCtrlInit(struct TagSwTmrCtrl *swtmr) { (void)swtmr; }  /* defined in test.c */
#endif

/* Tickless function pointers */
typedef U64 (*GetSwtmrNearestTickFunc)(void);
typedef bool (*CheckSwtmrTickProcessFunc)(U64 targetTick);
GetSwtmrNearestTickFunc g_getSwtmrNearestTick = NULL;
CheckSwtmrTickProcessFunc g_checkSwtmrTickProcess = NULL;

/* Tickless functions - weak implementations */
#ifndef OS_SWTMR_NEAREST_TICK_GET_DEFINED
__attribute__((weak)) U64 OsSwtmrNearestTickGet(U32 coreId) { (void)coreId; return 0; }
#endif
#ifndef OS_SWTMR_TARGET_CHECK_DEFINED
__attribute__((weak)) bool OsSwtmrTargetCheck(U32 coreId) { (void)coreId; return false; }
#endif

/* Error goto macro */
#define OS_GOTO_SYS_ERROR1() do { \
    if (g_tmrSortLink.sortLink) PRT_MemFree((U32)OS_MID_SWTMR, (void *)g_tmrSortLink.sortLink); \
    return OS_ERRNO_SWTMR_NO_MEMORY; \
} while(0)

/* DIV64 macro for 64-bit division */
#define DIV64(a, b) ((U32)((a) / (b)))
#define DIV64_REMAIN(a, b) ((U32)((a) % (b)))

/* ============================================================
 * Ring Buffer Declarations (implementations in test.c)
 * ============================================================ */
extern U32 PRT_RingbufUsedSize(Ringbuf *rb);
#ifndef OS_RINGBUF_INIT_DEFINED
__attribute__((weak)) U32 PRT_RingbufInit(Ringbuf *ringbuf, char *fifo, U32 size) {
    if (!ringbuf || !fifo) return OS_ERROR;
    ringbuf->fifo = fifo;
    ringbuf->size = size;
    ringbuf->remain = size;
    ringbuf->startIdx = 0;
    ringbuf->endIdx = 0;
    ringbuf->status = RBUF_INITED;
    return OS_OK;
}
#endif
#ifndef OS_RINGBUF_READ_DEFINED
__attribute__((weak)) U32 PRT_RingbufRead(Ringbuf *ringbuf, char *buf, U32 size) {
    (void)ringbuf; (void)buf; (void)size;
    return 0;
}
#endif
#ifndef OS_RINGBUF_WRITE_DEFINED
__attribute__((weak)) U32 PRT_RingbufWrite(Ringbuf *ringbuf, const char *src, U32 size) {
    (void)ringbuf; (void)src; (void)size;
    return 0;
}
#endif
#ifndef OS_RINGBUF_READ_LOOP_DEFINED
__attribute__((weak)) U32 OsRingbufReadLoop(Ringbuf *ringbuf, char *buf, U32 size) { (void)ringbuf; (void)buf; (void)size; return 0; }
#endif
#ifndef OS_RINGBUF_WRITE_LOOP_DEFINED
__attribute__((weak)) U32 OsRingbufWriteLoop(Ringbuf *ringbuf, const char *src, U32 size) { (void)ringbuf; (void)src; (void)size; return 0; }
#endif
#ifndef OS_RINGBUF_READ_LINEAR_DEFINED
extern U32 OsRingbufReadLinear(Ringbuf *ringbuf, char *buf, U32 size);
__attribute__((weak)) U32 OsRingbufReadLinear(Ringbuf *ringbuf, char *buf, U32 size) { (void)ringbuf; (void)buf; (void)size; return 0; }
#endif
#ifndef OS_RINGBUF_WRITE_LINEAR_DEFINED
extern U32 OsRingbufWriteLinear(Ringbuf *ringbuf, const char *src, U32 size);
__attribute__((weak)) U32 OsRingbufWriteLinear(Ringbuf *ringbuf, const char *src, U32 size) { (void)ringbuf; (void)src; (void)size; return 0; }
#endif

/* ============================================================
 * Spinlock Stubs
 * ============================================================ */
#ifndef PRT_SPL_LOCK_DEFINED
static inline uintptr_t PRT_SplLock(PrtSpinLock *lock) { (void)lock; return 0; }
#endif
#ifndef PRT_SPL_UNLOCK_DEFINED
static inline void PRT_SplUnlock(PrtSpinLock *lock, uintptr_t flags) { (void)lock; (void)flags; }
#endif
#ifndef PRT_SPL_LOCK_INIT_DEFINED
static inline U32 PRT_SplLockInit(PrtSpinLock *lock) { (void)lock; return OS_OK; }
#endif
__attribute__((weak)) U32 OsSplLockInit(PrtSpinLock *lock) { (void)lock; return OS_OK; }
#ifndef PRT_SPL_IRQ_LOCK_DEFINED
static inline uintptr_t PRT_SplIrqLock(PrtSpinLock *lock) { (void)lock; return 0; }
#endif
#ifndef PRT_SPL_IRQ_UNLOCK_DEFINED
static inline void PRT_SplIrqUnlock(PrtSpinLock *lock, uintptr_t flags) { (void)lock; (void)flags; }
#endif
#ifndef PRT_INT_LOCK_DEFINED
static inline uintptr_t PRT_IntLock(void) { return 0; }
#endif

/* OsSplLock/Unlock weak implementations */
__attribute__((weak)) void OsSplLock(SplLock *lock) { (void)lock; }
__attribute__((weak)) void OsSplUnlock(SplLock *lock) { (void)lock; }
__attribute__((weak)) void OsSplReadLock(volatile uintptr_t *addr) { (void)addr; }
__attribute__((weak)) void OsSplReadUnlock(volatile uintptr_t *addr) { (void)addr; }
__attribute__((weak)) void OsSplWriteLock(volatile uintptr_t *addr) { (void)addr; }
__attribute__((weak)) void OsSplWriteUnlock(volatile uintptr_t *addr) { (void)addr; }
#ifndef OS_RESORT_SEM_LIST_DEFINED
__attribute__((weak)) void OsResortSemList(struct TagTskCb *task) { (void)task; }
#endif
__attribute__((weak)) struct TagOsRunQue *OsSpinLockRunTaskRq(void) { return NULL; }
__attribute__((weak)) void OsSpinUnLockRunTaskRq(struct TagOsRunQue *rq) { (void)rq; }
#define OS_TSK_LOCK() do {} while(0)

/* Semaphore CB IRQ lock macros */
#define SEM_CB_IRQ_LOCK(cb, flags) do { (void)cb; flags = PRT_IntLock(); } while(0)
#define SEM_CB_IRQ_UNLOCK(cb, flags) do { (void)cb; PRT_IntRestore(flags); } while(0)
#define SEM_CB_UNLOCK(cb) do { (void)cb; } while(0)

/* Semaphore functions */
__attribute__((weak)) void OsSemIfPrioLock(void *sem) { (void)sem; }
__attribute__((weak)) void OsSemIfPrioUnLock(void *sem) { (void)sem; }
#ifndef OS_SEM_PEND_PARA_CHECK_DEFINED
__attribute__((weak)) U32 OsSemPendParaCheck(U32 timeout) { (void)timeout; return OS_OK; }
#endif
#ifndef OS_SEM_PEND_NOT_NEED_SCHE_DEFINED
__attribute__((weak)) U32 OsSemPendNotNeedSche(void *sem, void *task) { (void)sem; (void)task; return FALSE; }
#endif
__attribute__((weak)) void OsTskScheduleFastPs(uintptr_t flags) { (void)flags; }
__attribute__((weak)) void OsTskScheduleFast(void) { }
__attribute__((weak)) void OsRescheduleCurr(struct TagOsRunQue *rq) { (void)rq; }
#ifndef OS_TASK_YIELD_PROC_DEFINED
__attribute__((weak)) U32 OsTaskYieldProc(TskHandle nextTaskId, TskPrior taskPrio, TskHandle *yieldTo, struct TagTskCb *currTask, struct TagListObject *tskPriorRdyList) { (void)nextTaskId; (void)taskPrio; (void)yieldTo; (void)currTask; (void)tskPriorRdyList; return OS_OK; }
#endif
#define OS_TASK_GET_PRI_LIST(prio) ((struct TagListObject *)NULL)
#define THIS_RUNQ() ((struct TagOsRunQue *)NULL)
#ifndef OS_SEM_POST_ERROR_CHECK_DEFINED
__attribute__((weak)) U32 OsSemPostErrorCheck(void *sem, void *handle) { (void)sem; (void)handle; return OS_OK; }
#endif
#ifndef OS_SEM_POST_IS_INVALID_DEFINED
__attribute__((weak)) U32 OsSemPostIsInvalid(void *sem) { (void)sem; return FALSE; }
#endif
#ifndef OS_SEM_PEND_LIST_GET_DEFINED
__attribute__((weak)) void OsSemPendListGet(void *sem) { (void)sem; }
#endif
__attribute__((weak)) U32 OsGetHwThreadId(void) { return 0; }
#ifndef OS_GET_SRC_PID_DEFINED
__attribute__((weak)) U32 OsGetSrcPid(void) { return 0; }
#endif
#define COMPOSE_PID(core, type) ((core << 16) | (type))
#define OS_QUEUE_INNER_ID(id) ((id) & 0xFFFFU)
#define GET_QUEUE_HANDLE(id) ((void *)(id))
#define QUEUE_CB_IRQ_LOCK(cb, flags) do { (void)cb; flags = PRT_IntLock(); } while(0)
#define QUEUE_CB_IRQ_UNLOCK(cb, flags) do { (void)cb; PRT_IntRestore(flags); } while(0)
#define QUEUE_CB_LOCK(cb) do { (void)cb; } while(0)
#define QUEUE_CB_UNLOCK(cb) do { (void)cb; } while(0)
#define OS_QUEUE_PID_INVALID 0xFFFFU
#define OS_TSK_UNLOCK() do {} while(0)

/* Queue functions */
#ifndef OS_INNER_PEND_DEFINED
__attribute__((weak)) U32 OsInnerPend(U32 *cnt, void *list, U32 timeout, void *cb) { (void)cnt; (void)list; (void)timeout; (void)cb; return OS_OK; }
#endif
#ifndef OS_QUEUE_PEND_NEED_PROC_DEFINED
__attribute__((weak)) U32 OsQueuePendNeedProc(void *list) { (void)list; return FALSE; }
#endif
#ifndef OS_QUEUE_WRITE_PARA_CHECK_DEFINED
__attribute__((weak)) U32 OsQueueWriteParaCheck(U32 innerId, uintptr_t bufferAddr, U32 bufferSize, U32 prio) { (void)innerId; (void)bufferAddr; (void)bufferSize; (void)prio; return OS_OK; }
#endif
#ifndef OS_QUEUE_CP_DATA_2_NODE_DEFINED
__attribute__((weak)) void OsQueueCpData2Node(U32 prio, uintptr_t bufferAddr, U32 bufferSize, void *queueCb) { (void)prio; (void)bufferAddr; (void)bufferSize; (void)queueCb; }
#endif
__attribute__((weak)) void *OsMemAlloc(U32 mid, U32 opt, U32 size) { (void)mid; (void)opt; (void)size; return NULL; }
__attribute__((weak)) U32 OsIdleTskSMPCreate(void) { return OS_OK; }
__attribute__((weak)) void OsSmpWakeUpSecondaryCore(void) { }
#ifndef OS_FIRST_TIME_SWITCH_DEFINED
__attribute__((weak)) void OsFirstTimeSwitch(void) { }
#endif
__attribute__((weak)) bool OsTaskStackIsSave(struct TagTskCb *taskCb) { (void)taskCb; return false; }
#ifndef OS_TASK_SP_PC_INFO_READY_DEFINED
__attribute__((weak)) bool OsTaskSpPcInfoReady(struct TagTskCb *taskCb) { (void)taskCb; return false; }
#endif
__attribute__((weak)) uintptr_t OsTskGetInstrAddr(uintptr_t stackPtr) { (void)stackPtr; return 0; }
__attribute__((weak)) uintptr_t OsGetSp(void) { return 0; }
#define THIS_CORE() (0)
#define OS_MODULE_ERRNO_UNIT_FULL 0x0A000001U
#define OS_MODULE_ERRNO_MEMCOPY 0x0A000002U
#define OS_MODULE_ERRNO_ELF_HEAD_MAGIC 0x0A000003U
#define OS_MODULE_ERRNO_ELF_HEAD_CLASS 0x0A000004U
#define OS_MODULE_ERRNO_ELF_HEAD_LEN 0x0A000005U
#define OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE 0x0A000006U
#define OS_MODULE_ERRNO_ELF_HEAD_SHNUM 0x0A000007U
#define OS_MODULE_ERRNO_ELF_HEAD_MACHINE 0x0A000008U
#define OS_MODULE_ERRNO_ELF_HEAD_TYPE 0x0A000009U
#define OS_MODULE_ERRNO_PTLOAD_SIZE 0x0A00000AU
#define OS_MODULE_ALIGN_LEN 0x1000U
#define MEM_ADDR_ALIGN_4K 0x1000U
#define MODULE_UNIT_FREE 0U
#define MODULE_UNIT_ACTIVE 2U
__attribute__((weak)) uintptr_t OsModuleIntLock(void) { return 0; }
__attribute__((weak)) void OsModuleIntUnlock(uintptr_t flags) { (void)flags; }
__attribute__((weak)) int vsnprintf_s(char *str, size_t sizeInBytes, size_t maxCount, const char *format, va_list ap) {
    (void)sizeInBytes;
    return vsnprintf(str, maxCount, format, ap);
}
__attribute__((weak)) Elf_Addr OsDynModuleFindSymFromOs(const char *name) { (void)name; return 0; }
__attribute__((weak)) U32 OsDynModuleRelocate(Elf_Addr relocAddr, struct OsDynModuleRelocInfo relocInfo) { (void)relocAddr; (void)relocInfo; return OS_OK; }
#ifndef OS_DYN_MODULE_RELOCATE_PREPARE_DEFINED
__attribute__((weak)) U32 OsDynModuleRelocatePrepare(struct DynModuleUnitInfo *unitInfo, uintptr_t reloc, struct DynSymbol *symTab, uint8_t *strTab, Elf_Shdr *shdr) { (void)unitInfo; (void)reloc; (void)symTab; (void)strTab; (void)shdr; return OS_OK; }
#endif
__attribute__((weak)) void os_asm_invalidate_dcache_all(void) { }
__attribute__((weak)) void os_asm_invalidate_icache_all(void) { }
#ifndef OS_DYN_MODULE_SET_ERROR_DEFINED
__attribute__((weak)) void OsDynModuleSetError(const char *fmt, ...) { }
#endif
#ifndef OS_CHECK_LOG_DEFINED
__attribute__((weak)) U32 OsCheckLog(U32 level, U32 facility) { (void)level; (void)facility; return FALSE; }
#endif
#ifndef OS_LOG_DEFINED
__attribute__((weak)) U32 OsLog(U32 level, U32 facility, const char *buff, U32 len) { (void)level; (void)facility; (void)buff; (void)len; return OS_OK; }
#endif
#ifndef OS_LOG_GET_TAIL_AND_HEAD_DEFINED
__attribute__((weak)) void OsLogGetTailAndHead(U32 *head, U32 *tail) { if (head) *head = 0; if (tail) *tail = 0; }
#endif
#ifndef OS_LOG_LOCK_ON_DEFINED
__attribute__((weak)) uintptr_t OsLogLockOn(void) { return 0; }
#endif
#ifndef OS_ATOMIC_READ_U32_DEFINED
__attribute__((weak)) U32 OsAtomicReadU32(volatile U32 *addr) { if (addr) return *addr; return 0; }
#endif
#ifndef OS_ATOMIC_SET_U32_DEFINED
__attribute__((weak)) void OsAtomicSetU32(volatile U32 *addr, U32 value) { if (addr) *addr = value; }
#endif
#ifndef OS_LOG_LOCK_RESTORE_DEFINED
__attribute__((weak)) void OsLogLockRestore(uintptr_t intSave) { (void)intSave; }
#endif
#ifndef OS_LOG_UPDATE_TAIL_DEFINED
__attribute__((weak)) U32 OsLogUpdateTail(U32 *tail) { if (tail) *tail = 0; return OS_OK; }
#endif
__attribute__((weak)) int clock_gettime(int clk_id, struct timespec *tp) { (void)clk_id; if (tp) { tp->tv_sec = 0; tp->tv_nsec = 0; } return 0; }
__attribute__((weak)) U32 OsDynModuleFileCheck(const char *moduleFile, void *fileStat, struct DynModuleUnitInfo **unitInfo) { (void)moduleFile; (void)fileStat; (void)unitInfo; return OS_OK; }
__attribute__((weak)) U32 OsDynModuleGetFileContent(const char *moduleFile, char **moduleStr) { (void)moduleFile; (void)moduleStr; return OS_OK; }
#ifndef OS_DYN_MODULE_CHECK_ELF_DEFINED
__attribute__((weak)) U32 OsDynModuleCheckElf(void *elfInfo) { (void)elfInfo; return OS_OK; }
#endif
#ifndef OS_DYN_MODULE_UNIT_ALLOC_DEFINED
__attribute__((weak)) U32 OsDynModuleUnitAlloc(struct DynModuleUnitInfo **dynModuleUnitInfo) { (void)dynModuleUnitInfo; return OS_OK; }
#endif
__attribute__((weak)) U32 OsDynModuleUnitLoad(struct DynModuleUnitInfo *unitInfo, U32 mode) { (void)unitInfo; (void)mode; return OS_OK; }
#ifndef PRT_INT_RESTORE_DEFINED
static inline void PRT_IntRestore(uintptr_t flags) { (void)flags; }
#endif

/* RwLock functions */
/* OsRwLockCheck, OsRwLockTryRdCheck, OsRwLockPost may be provided by test.c */
extern U32 OsRwLockCheck(void *rwl);
__attribute__((weak)) U32 OsRwLockCheck(void *rwl) { (void)rwl; return OS_OK; }
#ifndef OS_RWLOCK_POST_DEFINED
extern U32 OsRwLockPost(void *rwl, bool *needSched);
__attribute__((weak)) U32 OsRwLockPost(void *rwl, bool *needSched) { (void)rwl; if (needSched) *needSched = false; return OS_OK; }
#endif
#ifndef OS_RWLOCK_TRY_RD_CHECK_DEFINED
extern U32 OsRwLockTryRdCheck(struct TagTskCb *task, struct TagRwLockCb *rwl);
__attribute__((weak)) U32 OsRwLockTryRdCheck(struct TagTskCb *task, struct TagRwLockCb *rwl) { (void)task; (void)rwl; return OS_OK; }
#endif
#ifndef OS_RWLOCK_PEND_FIND_POS_SUB_DEFINED
extern U32 OsRwLockPendFindPosSub(struct TagTskCb *task, TagRwLockCb *rwl);
__attribute__((weak)) U32 OsRwLockPendFindPosSub(struct TagTskCb *task, TagRwLockCb *rwl) { (void)task; (void)rwl; return OS_OK; }
#endif
#ifndef OS_RWLOCK_PEND_FIND_POS_DEFINED
extern U32 OsRwLockPendFindPos(struct TagTskCb *task, TagRwLockCb *rwl);
__attribute__((weak)) U32 OsRwLockPendFindPos(struct TagTskCb *task, TagRwLockCb *rwl) { (void)task; (void)rwl; return 0; }
#endif
#ifndef OS_RWLOCK_PEND_PRE_DEFINED
extern void OsRwLockPendPre(struct TagTskCb *runTask, struct TagListObject *list, U32 timeout, pthread_rwlock_t *rwl);
__attribute__((weak)) void OsRwLockPendPre(struct TagTskCb *runTask, struct TagListObject *list, U32 timeout, pthread_rwlock_t *rwl) { (void)runTask; (void)list; (void)timeout; (void)rwl; }
#endif
#ifndef OS_RWLOCK_GET_MODE_DEFINED
extern U32 OsRwLockGetMode(struct TagListObject *readList, struct TagListObject *writeList);
__attribute__((weak)) U32 OsRwLockGetMode(struct TagListObject *readList, struct TagListObject *writeList) { (void)readList; (void)writeList; return RWLOCK_NONE_MODE; }
#endif
/* OsRwlockTryWrCheck may be provided by test.c */
#ifndef OS_RWLOCK_TRY_WR_CHECK_DEFINED
extern U32 OsRwlockTryWrCheck(struct TagTskCb *task, struct TagRwLockCb *rwl);
__attribute__((weak)) U32 OsRwlockTryWrCheck(struct TagTskCb *task, struct TagRwLockCb *rwl) { (void)task; (void)rwl; return OS_OK; }
#endif
#ifndef OS_RWLOCK_TASK_WAKE_DEFINED
extern void OsRwLockTaskWake(struct TagTskCb *task);
__attribute__((weak)) void OsRwLockTaskWake(struct TagTskCb *task) { (void)task; }
#endif
#ifndef OS_RWLOCK_PEND_SCHEDULE_DEFINED
extern U32 OsRwLockPendSchedule(struct TagTskCb *runTask, struct TagListObject *readList, U32 timeout, U32 intSave, pthread_rwlock_t *rwl);
__attribute__((weak)) U32 OsRwLockPendSchedule(struct TagTskCb *runTask, struct TagListObject *readList, U32 timeout, U32 intSave, pthread_rwlock_t *rwl) { (void)runTask; (void)readList; (void)timeout; (void)intSave; (void)rwl; return OS_OK; }
#endif
#ifndef OS_RWLOCK_PRI_COMPARE_DEFINED
extern bool OsRwlockPriCompare(struct TagTskCb *task, struct TagListObject *writeList);
__attribute__((weak)) bool OsRwlockPriCompare(struct TagTskCb *task, struct TagListObject *writeList) { (void)task; (void)writeList; return false; }
#endif

/* Notifier chain functions */
#ifndef NOTIFIER_CALL_CHAIN_DEFINED
extern int NotifierCallChain(struct NotifierBlock **nl, int val, void *v, int nr_to_call, int *nr_calls);
__attribute__((weak)) int NotifierCallChain(struct NotifierBlock **nl, int val, void *v, int nr_to_call, int *nr_calls) { (void)nl; (void)val; (void)v; (void)nr_to_call; (void)nr_calls; return 0; }
#endif
#ifndef NOTIFIER_CHAIN_REGISTER_DEFINED
extern int NotifierChainRegister(struct NotifierBlock **nl, struct NotifierBlock *n);
__attribute__((weak)) int NotifierChainRegister(struct NotifierBlock **nl, struct NotifierBlock *n) { (void)nl; (void)n; return 0; }
#endif

/* Global notifier chain for die events */
#ifndef G_DIE_NOTIFIER_DEFINED
#define G_DIE_NOTIFIER_DEFINED
extern NotifierHead g_dieNotifier;
__attribute__((weak)) NotifierHead g_dieNotifier = {{0}};
#endif

#ifndef OS_NOTIFIER_CHAIN_REGISTER_DEFINED
extern int OsNotifierChainRegister(NotifierHead *h, struct NotifierBlock *n);
__attribute__((weak)) int OsNotifierChainRegister(NotifierHead *h, struct NotifierBlock *n) { (void)h; (void)n; return 0; }
#endif

static inline uintptr_t PRT_HwiLock(void) { return 0; }
static inline void PRT_HwiRestore(uintptr_t intSave) { (void)intSave; }

/* ============================================================
 * Secure memset - must be non-static for linking
 * ============================================================ */
int memset_s(void *dest, size_t destsz, int ch, size_t count) {
    if (!dest) return (int)((U32)'E' | ((U32)'N' << 8) | ((U32)'O' << 16) | ((U32)'S' << 24));
    if (count > destsz) return (int)((U32)'E' | ((U32)'R' << 8) | ((U32)'N' << 16) | ((U32)'O' << 24));
    memset(dest, ch, count);
    return 0;
}

U32 memcpy_s(void *dest, size_t destsz, const void *src, size_t count) {
    if (!dest || !src || count > destsz) return 1;
    memcpy(dest, src, count);
    return 0;
}

/* ============================================================
 * Error logging stubs
 * ============================================================ */
#define OS_ERR_LEVEL_HIGH 1
#define OS_ERROR_LOG_REPORT(level, fmt, ...) printf(fmt, ##__VA_ARGS__)

/* ============================================================
 * Additional kernel function stubs
 * ============================================================ */
static inline U32 PRT_GetCoreID(void) { return 0; }
static inline U32 PRT_TaskSelf(TskHandle *handle) { 
    if (handle) *handle = 1;
    return OS_OK; 
}
static inline U64 PRT_ClkGetCycleCount64(void) { return 0; }
#ifndef OS_CUR_CYCLE_GET_64_DEFINED
extern U64 OsCurCycleGet64(void);
__attribute__((weak)) U64 OsCurCycleGet64(void) { return 0; }
#endif
static inline void OsSpinLockInitInner(SplLock *lock) { (void)lock; }
#define OS_MS2CYCLE(ms, clock) ((U64)(ms) * (U64)(clock) / 1000U)
#define OS_US2CYCLE(us, clock) ((U64)(us) * (U64)(clock) / 1000000U)
static inline U32 OsPerfBackTrace(uintptr_t *buf, U32 depth, PerfRegs *regs) { 
    (void)buf; (void)depth; (void)regs;
    return 0; 
}
static inline void PRT_Printf(const char *fmt, ...) { 
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/* Perf output functions - declarations only (test.c provides implementations) */
#ifndef OS_PERF_OUTPUT_FLUSH_DEFINED
#define OS_PERF_OUTPUT_FLUSH_DEFINED
extern void OsPerfOutPutFlush(void);
#endif
#ifndef OS_PERF_DEFAULT_NOTIFY_DEFINED
#define OS_PERF_DEFAULT_NOTIFY_DEFINED
extern void OsPerfDefaultNotify(void);
#endif
#ifndef OS_PERF_PRINT_COUNT_DEFINED
#define OS_PERF_PRINT_COUNT_DEFINED
extern void OsPerfPrintCount(void);
#endif
static inline bool OsPerfOutPutBegin(U32 size) {
    (void)size;
    return true;
}
static inline void OsPerfOutPutEnd(void) {
    /* Empty implementation */
}
#ifndef OS_PERF_OUT_PUT_READ_DEFINED
#define OS_PERF_OUT_PUT_READ_DEFINED
extern U32 OsPerfOutPutRead(char *dest, U32 size);
#endif
#ifndef OS_PERF_FLUSH_HOOK_REG_DEFINED
#define OS_PERF_FLUSH_HOOK_REG_DEFINED
extern void OsPerfFlushHookReg(PERF_BUF_FLUSH_HOOK func);
#endif
#ifndef OS_PMU_INIT_DEFINED
#define OS_PMU_INIT_DEFINED
extern U32 OsPmuInit(void);
#endif
#ifndef OS_PERF_OUTPUT_INIT_DEFINED
#define OS_PERF_OUTPUT_INIT_DEFINED
extern U32 OsPerfOutPutInit(void *buf, U32 size);
#endif
#ifndef OS_PERF_NOTIFY_HOOK_REG_DEFINED
#define OS_PERF_NOTIFY_HOOK_REG_DEFINED
extern void OsPerfNotifyHookReg(PERF_BUF_NOTIFY_HOOK func);
#endif
#ifndef OS_PERF_PARAM_VALID_DEFINED
__attribute__((weak)) bool OsPerfParamValid(void) {
    return FALSE;
}
#endif
#ifndef OS_PERF_HDR_INIT_DEFINED
__attribute__((weak)) U32 OsPerfHdrInit(U32 sectionId) {
    (void)sectionId;
    return OS_OK;
}
#endif
#ifndef OS_PERF_START_DEFINED
__attribute__((weak)) void OsPerfStart(void) { }
#endif
#define SMP_CALL_PERF_FUNC(func) func()

#ifndef OS_IDLE_TASK_EXE_DEFINED
__attribute__((weak)) void OsIdleTaskExe(void) { }
#endif

#ifndef OS_GET_PEND_TSK_MAX_PRIORITY_DEFINED
static inline void OsGetPendTskMaxPriority(struct TagTskCb *taskCb, TskPrior *maxPrio) {
    (void)taskCb;
    if (maxPrio) *maxPrio = 0;
}
#endif

/* ============================================================
 * Missing function stub implementations
 * ============================================================ */
#ifndef OS_TASK_CREATE_ONLY_DEFINED
static inline U32 OsTaskCreateOnly(TskHandle *taskPid, struct TskInitParam *param, U32 unused) {
    (void)taskPid; (void)param; (void)unused;
    return OS_OK;
}
#endif
#ifndef OS_TASK_CREATE_PARA_CHECK_DEFINED
__attribute__((weak)) U32 OsTaskCreateParaCheck(U32 *taskPid, struct TskInitParam *param) { (void)taskPid; (void)param; return OS_OK; }
#endif
__attribute__((weak)) bool OsCheckStackAddrOverflow(uintptr_t stackAddr, U32 stackSize) { (void)stackAddr; (void)stackSize; return false; }
#define OS_ERRNO_TSK_STKSZ_NOT_ALIGN 0x01000001U
#define OS_ERRNO_TSK_STACKADDR_NOT_ALIGN 0x01000002U
#define OS_TSK_MIN_STACK_SIZE 256U
#define OS_ERRNO_TSK_STKSZ_TOO_SMALL 0x01000003U
#define OS_ERRNO_TSK_STACKADDR_TOO_BIG 0x01000004U
#define OS_ERRNO_TSK_NAME_EMPTY 0x01000005U
__attribute__((weak)) U32 OsTaskCreateChkAndGetTcb(struct TagTskCb **taskCb, bool isSmpIdle) { (void)taskCb; (void)isSmpIdle; return OS_OK; }
__attribute__((weak)) U32 OsTaskCreateRsrcInit(U32 taskId, struct TskInitParam *param, struct TagTskCb *taskCb, void **topStack, U32 *curStackSize) { (void)taskId; (void)param; (void)taskCb; (void)topStack; (void)curStackSize; return OS_OK; }
#ifndef OS_TASK_MANAGE_FREE_CB_DEFINED
__attribute__((weak)) void OsTaskManageFreeCb(struct TagTskCb *taskCb) { (void)taskCb; }
#endif
#define TSK_CREATE_DEL_LOCK() do {} while(0)
#define TSK_CREATE_DEL_UNLOCK() do {} while(0)
#define EOK 0

__attribute__((weak)) int strncpy_s(char *dest, size_t destsz, const char *src, size_t count) {
    if (dest == NULL || src == NULL || destsz == 0) return -1;
    size_t i;
    for (i = 0; i < destsz - 1 && i < count && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return 0;
}
#ifndef OS_TSK_STACK_INIT_DEFINED
__attribute__((weak)) void OsTskStackInit(U32 curStackSize, uintptr_t topStack) { (void)curStackSize; (void)topStack; }
#endif
#ifndef OS_TSK_CREATE_TCB_INIT_DEFINED
__attribute__((weak)) void OsTskCreateTcbInit(uintptr_t stackPtr, struct TskInitParam *param, uintptr_t topStack, U32 curStackSize, struct TagTskCb *taskCb) { (void)stackPtr; (void)param; (void)topStack; (void)curStackSize; (void)taskCb; }
#endif
#ifndef OS_TSK_CREATE_TCB_STATUS_SET_DEFINED
__attribute__((weak)) void OsTskCreateTcbStatusSet(struct TagTskCb *taskCb, struct TskInitParam *param) { (void)taskCb; (void)param; }
#endif
#ifndef OS_QUEUE_CREAT_PARA_CHECK_DEFINED
static inline U32 OsQueueCreatParaCheck(U32 queueId, U32 nMsg, U32 nByte) {
    (void)queueId; (void)nMsg; (void)nByte;
    return OS_OK;
}
#endif
#ifndef OS_QUEUE_GET_NODE_SIZE_DEFINED
__attribute__((weak)) U32 OsQueueGetNodeSize(U16 nodeSize) { (void)nodeSize; return 0; }
#endif
#define ALIGN(size, align) (((size) + (align) - 1) & ~((align) - 1))
#ifndef OS_QUEUE_CREATE_DEFINED
static inline U32 OsQueueCreate(U32 queueId, U32 nMsg, U32 nByte, U32 flags, QueueHandle *queueHandle) {
    (void)queueId; (void)nMsg; (void)nByte; (void)flags;
    if (queueHandle) *queueHandle = queueId;
    return OS_OK;
}
#endif
#define OS_QUEUE_ID(id) ((id) | (OS_MID_QUEUE << 16))
#define OS_QUEUE_USED 1U
#define OS_ERRNO_QUEUE_CREATE_NO_MEMORY 0x05000010U
#define INIT_LIST_OBJECT(list) do { (list)->next = (list); (list)->prev = (list); } while(0)
#ifndef OS_DYN_MODULE_UNIT_FREE_DEFINED
static inline U32 OsDynModuleUnitFree(struct DynModuleUnitInfo *unitInfo) {
    (void)unitInfo;
    return OS_OK;
}
#endif
static inline void OsTaskScan(void) { }
static inline void OsTaskDelay(U32 ms) { (void)ms; }
static inline void OsTaskDelayUntil(U64 wakeTime) { (void)wakeTime; }
static inline void OsTaskPriModify(TskHandle taskId, TskPrior newPrio) { (void)taskId; (void)newPrio; }
static inline U32 OsTaskPriSet(TskHandle taskId, TskPrior newPrio) { (void)taskId; (void)newPrio; return OS_OK; }
static inline U32 OsTaskPriGet(TskHandle taskId, TskPrior *prio) { (void)taskId; if (prio) *prio = 0; return OS_OK; }
static inline void OsTaskDelete(TskHandle taskId) { (void)taskId; }
static inline void OsTaskResume(TskHandle taskId) { (void)taskId; }
static inline U32 OsTaskSuspend(TskHandle taskId) { (void)taskId; return OS_OK; }
#ifndef OS_TASK_YIELD_DEFINED
static inline U32 OsTaskYield(TskPrior taskPrio, TskHandle nextTask, U32 yieldTo, uintptr_t intSave) { (void)taskPrio; (void)nextTask; (void)yieldTo; (void)intSave; return OS_OK; }
#endif
static inline U32 OsTaskInfoGet(TskHandle taskId, struct TagTskCb **tskCb) { (void)taskId; if (tskCb) *tskCb = NULL; return OS_OK; }
static inline U32 OsTaskStackInfo(TskHandle taskId, struct TagTskCb *tskCb, struct TskInfo **info) { (void)taskId; (void)tskCb; if (info) *info = NULL; return OS_OK; }
#ifndef OS_TSK_ENTRY_DEFINED
static inline void OsTskEntry(U32 arg) { (void)arg; while(1); }
#endif

/* Queue IRQ lock macros */
#define QUEUE_INIT_IRQ_LOCK(flag) ((flag) = 0)
#define QUEUE_INIT_IRQ_UNLOCK(flag) ((void)(flag))

/* ============================================================
 * Global variable definitions
 * ============================================================ */
extern U32 g_maxQueue;
extern struct TagQueCb *g_allQueue;
extern TaskScanHook g_taskScanHook;
extern struct TagListObject g_tskCbFreeList;
U32 g_maxQueue = 16;
struct TagQueCb *g_allQueue = NULL;
TaskScanHook g_taskScanHook = NULL;
struct TagListObject g_tskCbFreeList = {&g_tskCbFreeList, &g_tskCbFreeList};
struct DynModuleUnitInfo **g_dynModuleInfoPool = NULL;
U32 g_primaryCoreId = 0;
U32 g_numOfCores = 1;
U32 UNI_FLAG = 0;
void *g_logMemBase = NULL;
U32 g_sequenceNum = 0;
void *libc_global_locale = NULL;
U32 g_logOn = 1;
U32 g_logFilter[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
PrtSpinLock g_logLock = {0};
char *g_dynModuleErrorStr = NULL;
struct TagTskCb *g_highestTask = NULL;
U32 g_uniTaskLock = 0;
OsVoidFunc g_taskCoreSleep = NULL;
PowerOffFuncT g_sysPowerOffHook = NULL;
bool g_sysPowerOffFlag = false;
SetOfflineFlagFuncT g_setOfflineFlagHook = NULL;
Pmu *g_pmu = NULL;
PerfTimed *g_perfTimed = NULL;
U32 g_swtPmuId = 0;
SplLock g_perfSpin = 0;

/* Perf lock macros */
#define PERF_LOCK(intSave) intSave = OsIntLock()
#define PERF_UNLOCK(intSave) OsIntRestore(intSave)

#ifndef OS_PERF_SW_TIMER_DEFINED
__attribute__((weak)) void OsPerfSwtimer(TimerHandle tmrHandle, U32 arg1, U32 arg2, U32 arg3, U32 arg4) {
    (void)tmrHandle; (void)arg1; (void)arg2; (void)arg3; (void)arg4;
}
#endif
#ifndef PRT_TIMER_CREATE_DEFINED
__attribute__((weak)) U32 PRT_TimerCreate(struct TimerCreatePara *para, U32 *timerId) {
    (void)para; (void)timerId;
    return OS_OK;
}
#endif
#ifndef PRT_TIMER_START_DEFINED
__attribute__((weak)) U32 PRT_TimerStart(U32 timerGroupId, U32 timerId) {
    (void)timerGroupId; (void)timerId;
    return OS_OK;
}
#endif
#ifndef PRT_TIMER_DELETE_DEFINED
__attribute__((weak)) U32 PRT_TimerDelete(U32 timerGroupId, U32 timerId) {
    (void)timerGroupId; (void)timerId;
    return OS_OK;
}
#endif
#ifndef OS_PERF_FETCH_IRQ_REGS_DEFINED
__attribute__((weak)) void OsPerfFetchIrqRegs(PerfRegs *regs) { (void)regs; }
#endif
#ifndef OS_PERF_UPDATE_EVENT_COUNT_DEFINED
__attribute__((weak)) void OsPerfUpdateEventCount(Event *event, U32 count) { (void)event; (void)count; }
#endif
#ifndef OS_PERF_HANDLE_OVER_FLOW_DEFINED
__attribute__((weak)) void OsPerfHandleOverFlow(Event *event, PerfRegs *regs) { (void)event; (void)regs; }
#endif
#ifndef OS_PERF_TIMED_CONFIG_DEFINED
__attribute__((weak)) U32 OsPerfTimedConfig(void) { return OS_OK; }
#endif
#ifndef OS_PERF_TIMED_START_DEFINED
__attribute__((weak)) U32 OsPerfTimedStart(void) { return OS_OK; }
#endif
#ifndef OS_PERF_TIMED_STOP_DEFINED
__attribute__((weak)) U32 OsPerfTimedStop(void) { return OS_OK; }
#endif
#ifndef OS_PERF_GET_EVENT_NAME_DEFINED
__attribute__((weak)) const char *OsPerfGetEventName(const void *event) { (void)event; return ""; }
#endif
#ifndef OS_PERF_PMU_REGISTER_DEFINED
__attribute__((weak)) U32 OsPerfPmuRegister(Pmu *pmu) { (void)pmu; return OS_OK; }
#endif
#ifndef OS_TSK_SWITCH_HOOK_CALLER_DEFINED
__attribute__((weak)) void OsTskSwitchHookCaller(U32 prevPid, U32 nextPid) { (void)prevPid; (void)nextPid; }
#endif
#ifndef OS_TSK_HIGHEST_SET_DEFINED
__attribute__((weak)) void OsTskHighestSet(void) { }
#endif
#ifndef OS_INT_ENABLE_DEFINED
__attribute__((weak)) void OsIntEnable(void) { }
#endif
#ifndef OS_INT_DISABLE_DEFINED
__attribute__((weak)) void OsIntDisable(void) { }
#endif
#ifndef OS_MAIN_SCHEDULE_DEFINED
__attribute__((weak)) void OsMainSchedule(void) { }
#endif
#ifndef OS_TIME_SLICE_UPDATE_DEFINED
__attribute__((weak)) void OsTimeSliceUpdate(struct TagTskCb *task, U64 currTime, U32 unused) { (void)task; (void)currTime; (void)unused; }
#endif

extern void (*g_tickDispatcher)(void);
#define OS_IRQ_TIME_RECORD(startTime) do {} while(0)

OsPerfTimedHandle g_perfTimedHandle = NULL;
void (*coreSleep)(void) = NULL;
void (*g_tickDispatcher)(void) = NULL;
U32 TICK_NO_RESPOND_CNT = 0;

/* Task info structure */
typedef struct TskInfo {
    U32 taskStatus;
    U32 stackSize;
    void *stackBase;
    void *stackPointer;
    uintptr_t sp;
    uintptr_t pc;
    TskPrior taskPrio;
    void *topOfStack;
    char name[OS_TSK_NAME_LEN];
    U32 core;
    void (*entry)(U32 arg);
    void *tcbAddr;
} TskInfo;

typedef U32 TskStatus;
void (*g_taskNameGet)(TskHandle taskHandle, char **taskName) = NULL;

/* Task init param structure */
typedef struct TskInitParam {
    void (*taskEntry)(U32, U32, U32, U32);
    U32 arg;
    U32 args[4];
    U32 stackSize;
    void *stackAddr;
    TskPrior priority;
    TskPrior taskPrio;
    char name[OS_TSK_NAME_LEN];
    U32 flags;
} TskInitParam;

/* Task module info structure */
typedef struct TskModInfo {
    U32 maxTaskNum;
    U32 maxSemNum;
    U32 maxQueueNum;
    U32 defaultSize;
    U32 maxNum;
    U32 idleStackSize;
    U32 magicWord;
    U32 timeSliceMs;
} TskModInfo;

/* Queue control block structure */
typedef struct QueNode {
    U16 size;
    U16 srcPid;
    char buf[1];
} QueNode;

typedef struct TagQueCb {
    struct TagListObject semBList;
    U32 queueId;
    U32 queueStat;
    U32 queueState;
    U32 msgNum;
    U32 maxMsgNum;
    U32 maxByte;
    char *queueBase;
    char *queue;
    U32 queueHead;
    U32 queueTail;
    char *readPointer;
    char *writePointer;
    U32 msgSize;
    U32 nodeSize;
    U32 nodeNum;
    U32 nodePeak;
    U32 readableCnt;
    U32 writableCnt;
    struct TagListObject readList;
    struct TagListObject writeList;
} TagQueCb;

struct TskModInfo g_tskModInfo = {10, 10, 10, 0, 0, 0, 0, 0};
U64 g_timeSliceCycle = 0;
TskEntryFunc g_tskIdleEntry = NULL;
#ifndef OS_TSK_IDLE_BGD_DEFINED
__attribute__((weak)) void OsTskIdleBgd(U32 arg) { (void)arg; }
#endif
__attribute__((weak)) U32 OsTskSMPInit(void) { return OS_OK; }
__attribute__((weak)) void OsTskSMPTcbInit(struct TskInitParam *param, struct TagTskCb *taskCb) { (void)param; (void)taskCb; }
__attribute__((weak)) U32 PRT_TaskGetInfo(TskHandle taskId, struct TskInfo *taskInfo) { (void)taskId; (void)taskInfo; return OS_OK; }
__attribute__((weak)) void OS_ERR_RECORD(U32 err) { (void)err; }
__attribute__((weak)) struct TagTskCb *OsGetCurrentTcb(void) { return NULL; }
__attribute__((weak)) void OsTaskExit(struct TagTskCb *taskCb) { (void)taskCb; }

/* Run queue structure */
typedef struct TagOsRunQue {
    struct TagListObject *head;
    struct TagTskCb *tskCurr;
    U32 count;
    U32 uniTaskLock;
    U32 needReschedule;
    U32 uniFlag;
} TagOsRunQue;

/* HWI function stubs */
#ifndef OS_HWI_FUNC_SET_DEFINED
#define OS_HWI_FUNC_SET_DEFINED
extern void OsHwiFuncSet(U32 irqNum, void (*handler)(U32 irqNum));
__attribute__((weak)) void OsHwiFuncSet(U32 irqNum, void (*handler)(U32 irqNum)) {
    (void)irqNum;
    (void)handler;
}
#endif

#ifndef OS_HWI_PARA_SET_DEFINED
#define OS_HWI_PARA_SET_DEFINED
extern void OsHwiParaSet(U32 irqNum, U32 para);
__attribute__((weak)) void OsHwiParaSet(U32 irqNum, U32 para) {
    (void)irqNum;
    (void)para;
}
#endif

#endif /* __QSEM_TEST_STUBS_H__ */
