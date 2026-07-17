#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Section and inline macros used in source
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE inline

    typedef uint32_t U32;

    typedef struct TagTskCb TagTskCb;
    typedef void (*_sa_handler)(int);

    typedef uint64_t signalSet;

    typedef struct
    {
        int si_signo;
    } signalInfo;

    typedef struct ListHead
    {
        struct ListHead *prev;
        struct ListHead *next;
    } ListHead;

    // Minimal list helpers compatible with usage pattern
    static inline void ListInit(ListHead *head)
    {
        head->next = head;
        head->prev = head;
    }

    static inline void ListTailAdd(ListHead *node, ListHead *head)
    {
        node->prev = head->prev;
        node->next = head;
        head->prev->next = node;
        head->prev = node;
    }

    static inline void ListDelete(ListHead *node)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->next = node->prev = NULL;
    }

#define LIST_COMPONENT(ptr, type, member) \
    ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#define LIST_FOR_EACH(pos, head, type, member)                                                 \
    for ((pos) = ((head)->next == (head)) ? NULL : LIST_COMPONENT((head)->next, type, member); \
         (pos) != NULL;                                                                        \
         (pos) = (((pos)->member.next == (head)) ? NULL : LIST_COMPONENT((pos)->member.next, type, member)))

    typedef struct sigInfoNode
    {
        ListHead siglist; // matches field name used in source
        signalInfo siginfo;
    } sigInfoNode;

// Stubs for memory allocation (tag ignored)
#define OS_MID_SIGNAL 0
#define OS_MEM_DEFAULT_FSC_PT 0
    static inline void *PRT_MemAlloc(U32 tag, U32 fsc, size_t size)
    {
        (void)tag;
        (void)fsc;
        return malloc(size);
    }
    static inline void PRT_MemFree(U32 tag, void *ptr)
    {
        (void)tag;
        free(ptr);
    }

    // Spinlocks and ints
    static inline uintptr_t OsIntLock(void) { return 0; }
    static inline void OsIntRestore(uintptr_t s) { (void)s; }
    static inline void OsSpinLockTaskRq(TagTskCb *t) { (void)t; }
    static inline void OsSpinUnlockTaskRq(TagTskCb *t) { (void)t; }

// Task and scheduling flags + helpers
#define OS_TSK_TIMEOUT 0x1
#define OS_TSK_WAIT_SIGNAL 0x2
#define OS_TSK_READY 0x4
#define OS_TSK_DELAY 0x8
#define OS_TSK_SIG_PAUSE 0x10
#define OS_TSK_DELAY_INTERRUPTIBLE 0x20
#define OS_TSK_HOLD_SIGNAL 0x40
#define OS_TSK_SUSPEND 0x80

#define TSK_STATUS_SET(t, f) ((t)->taskStatus |= (f))
#define TSK_STATUS_CLEAR(t, f) ((t)->taskStatus &= ~(f))

#define OS_SIGNAL_WAIT_FOREVER 0xFFFFFFFFu

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SIG_IGN ((void (*)(int)) - 1)
#define SIG_DFL ((void (*)(int))0)

#define OS_OK 0
#define OS_ERRNO_SIGNAL_PARA_INVALID 1
#define OS_ERRNO_SIGNAL_WAIT_IN_LOCK 2
#define OS_ERRNO_SIGNAL_TIMEOUT 3
#define OS_ERRNO_SIGNAL_NUM_INVALID 4
#define OS_ERRNO_SIGNAL_TSK_NOT_CREATED 5
#define OS_ERRNO_SIGNAL_TASKID_INVALID 6
#define OS_ERRNO_SIGNAL_NO_MEMORY 7
#define OS_ERRNO_SIGNAL_WAIT_NOT_IN_TASK 8

#define OS_TASK_LOCK_DATA 0
#define OS_INT_ACTIVE 0

#define PRT_SIGNAL_MAX 64

#define IDLE_TASK_ID 0

    typedef unsigned int TskHandle;

    struct TagTskCb
    {
        U32 taskPid;
        U32 taskStatus;
        uintptr_t *stackPointer;
        uintptr_t *oldStackPointer;
        uintptr_t *topOfStack;
        _sa_handler sigVectors[PRT_SIGNAL_MAX];
        signalSet sigMask;
        signalSet sigPending;
        signalSet sigWaitMask;
        ListHead sigInfoList;
        int holdSignal;
        void *kthreadTsk; // unused in tests
    };

    // Global running task for tests
    extern struct TagTskCb g_runningTask;
    void StubResetTask(void);
#define RUNNING_TASK (&g_runningTask)

// TCB helpers
#define GET_TCB_HANDLE(id) (&g_runningTask)
#define CHECK_TSK_PID_OVERFLOW(id) (0)
#define TSK_IS_UNUSED(t) (0)

    static inline void OsTskReadyDel(TagTskCb *t) { (void)t; }
    static inline void OsTskReadyAdd(TagTskCb *t) { (void)t; }
    static inline void OsTskReadyAddBgd(TagTskCb *t) { (void)t; }
    static inline void OsTskSchedule(void) {}
    static inline void OsTskTimerAdd(TagTskCb *t, U32 to)
    {
        (void)t;
        (void)to;
    }
    static inline uintptr_t *OsTskContextInit(U32 taskId, U32 curStackSize, uintptr_t *top, uintptr_t entry)
    {
        (void)taskId;
        (void)curStackSize;
        (void)top;
        (void)entry;
        return NULL;
    }
    static inline void OsTskContextLoad(uintptr_t tsk) { (void)tsk; }
    static inline void OS_TSK_DELAY_LOCKED_DETACH(TagTskCb *t) { (void)t; }

    // Signal helpers
    static inline int sigValid(int s) { return s >= 0 && s < (int)PRT_SIGNAL_MAX; }
    static inline signalSet sigMask(int s) { return (signalSet)1ull << (s & 63); }

// memcpy_s, memset_s emulation
#define EOK 0
    __attribute__((weak)) int memcpy_s(void *dst, size_t dsz, const void *src, size_t ssz)
    {
        if (dsz < ssz)
            return -1;
        memcpy(dst, src, ssz);
        return EOK;
    }
    static inline int memset_s(void *dst, size_t dsz, int c, size_t sz)
    {
        if (dsz < sz)
            return -1;
        memset(dst, c, sz);
        return EOK;
    }

// System error macro stubbed to no-op for tests
#ifndef OS_GOTO_SYS_ERROR1
#define OS_GOTO_SYS_ERROR1() \
    do                       \
    {                        \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif
