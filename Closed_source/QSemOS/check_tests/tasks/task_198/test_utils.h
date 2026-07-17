// Minimal standalone test utilities and stubs to compile RTOS function tests
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

// Basic RTOS-like typedefs
typedef uint32_t U32;
typedef uint64_t U64;
typedef uint8_t U8;
typedef uint32_t TskHandle;
typedef void (*TskEntryFunc)(uintptr_t, uintptr_t, uintptr_t, uintptr_t);

// Locale stub
typedef void *locale_t;
extern void *libc_global_locale;

// BOOL definitions
#define TRUE 1
#define FALSE 0

// Sections/macros no-op
#define OS_SEC_DATA
#define OS_SEC_BSS
#define OS_SEC_L4_DATA
#define OS_SEC_L4_TEXT
#define OS_SEC_ALW_INLINE
#define INLINE

// Constants/config
#ifndef MAX_TASK_NUM
#define MAX_TASK_NUM 1024u
#endif
#define OS_OK 0u

// Error codes (dummy but stable values)
#define OS_ERRNO_TSK_MAX_NUM_NOT_SUITED 0x1001u
#define OS_ERRNO_TSK_PTR_NULL 0x1002u
#define OS_ERRNO_TSK_ENTRY_NULL 0x1003u
#define OS_ERRNO_TSK_STKSZ_NOT_ALIGN 0x1004u
#define OS_ERRNO_TSK_STACKADDR_NOT_ALIGN 0x1005u
#define OS_ERRNO_TSK_STKSZ_TOO_SMALL 0x1006u
#define OS_ERRNO_TSK_STACKADDR_TOO_BIG 0x1007u
#define OS_ERRNO_TSK_PRIOR_ERROR 0x1008u
#define OS_ERRNO_TSK_NAME_EMPTY 0x1009u
#define OS_ERRNO_TSK_NO_MEMORY 0x100Au
#define OS_ERRNO_TSK_ACTIVE_FAILED 0x100Bu
#define OS_ERRNO_SYS_GENERAL 0x1FFFu

// Misc constants
#define OS_SYS_MS_PER_SECOND 1000u
#define OS_TSK_DEFAULT_TIME_SLICE_MS 10u
#define OS_TSK_STACK_SIZE_ALLOC_ALIGN 16u
#define OS_TSK_STACK_SIZE_ALIGN 16u
#define OS_TSK_MIN_STACK_SIZE 64u
#define OS_TSK_PRIORITY_LOWEST 255u
#define OS_TSK_STACK_MAGIC 0xCCCCCCCCu
#define OS_TSK_STACK_TOP_MAGIC 0xDEADBEEFu
#define OS_TSK_PARA_0 0
#define OS_TSK_PARA_1 1
#define OS_TSK_PARA_2 2
#define OS_TSK_PARA_3 3
#define OS_TSK_SCHED_RR 1u
#define OS_TSK_SCHED_FIFO 0u
#define OS_TSK_SUSPEND 0x1u
#define OS_TSK_INUSE 0x2u

#define OS_MEM_DEFAULT_FSC_PT 0u

// Flags
extern U32 UNI_FLAG;
#define OS_FLG_TSK_SWHK (1u << 0)
#define OS_FLG_BGD_ACTIVE (1u << 1)

// Hook macros (no-op)
#define OS_MHOOK_ACTIVATE_PARA0(x) \
    do                             \
    {                              \
        (void)(x);                 \
    } while (0)
#define OS_MHOOK_ACTIVATE_PARA1(x, y) \
    do                                \
    {                                 \
        (void)(x);                    \
        (void)(y);                    \
    } while (0)

// Error record macro
#define OS_ERR_RECORD(x) (x)

// Simple secure strncpy_s stub
#ifndef EOK
#define EOK 0
#endif
static inline int strncpy_s(char *dest, size_t dmax, const char *src, size_t count)
{
    if (!dest || !src || dmax == 0)
        return 1;
    size_t slen = strlen(src);
    if (count > dmax - 1)
        count = dmax - 1; // ensure space for NUL
    if (slen > count)
    {
        // would truncate beyond allowed count -> error
        // but still copy count chars to be safe
        memcpy(dest, src, count);
        dest[count] = '\0';
        return 1;
    }
    memcpy(dest, src, slen);
    dest[slen] = '\0';
    return EOK;
}

// Forward declarations for stubs used by functions
struct TagTskCb;

// Doubly-linked list node
struct TagListObject
{
    struct TagListObject *prev;
    struct TagListObject *next;
};

#define LIST_OBJECT_INIT(name) {&(name), &(name)}

static inline void INIT_LIST_OBJECT(struct TagListObject *obj)
{
    obj->prev = obj->next = obj;
}

static inline int ListEmpty(struct TagListObject *head)
{
    return head->next == head;
}

static inline struct TagListObject *OS_LIST_FIRST(struct TagListObject *head)
{
    return head->next;
}

static inline void ListAdd(struct TagListObject *node, struct TagListObject *head)
{
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

static inline void ListTailAdd(struct TagListObject *node, struct TagListObject *head)
{
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

static inline void ListDelete(struct TagListObject *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = node; // isolate
}

#define OS_LIST_INIT(p) INIT_LIST_OBJECT((p))

// offsetof/CONTAINER_OF for getting TCB from list node
#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#define GET_TCB_PEND(listNode) container_of((listNode), struct TagTskCb, pendList)

// Structures used by functions
struct TskModInfo
{
    U32 maxNum;
    U32 defaultSize;
    U32 idleStackSize;
    U32 magicWord;
    U32 timeSliceMs;
};

struct TskInfo
{ // minimal placeholder
    int dummy;
};

struct TskInitParam
{
    TskEntryFunc taskEntry;
    uintptr_t args[4];
    U32 taskPrio;
    const char *name;
    U32 policy;
    uintptr_t stackAddr; // top address in original code usage
    U32 stackSize;
};

// TCB definition
struct TagTskCb
{
    // name and id
    char name[32];
    U32 taskPid;

    // lists
    struct TagListObject semBList;
    struct TagListObject pendList;
    struct TagListObject timerList;

    // stack
    void *stackPointer;
    uintptr_t args[4];
    uintptr_t topOfStack;
    uintptr_t stackSize;
    void *taskPend;

    // scheduling
    U32 priority;
    U32 origPriority;
    TskEntryFunc taskEntry;
    U32 policy;
    U32 startTime;
    U32 timeSlice;
    U32 irqUsedTime;
    U32 taskStatus;

    // events
    U32 event;
    U32 eventMask;

    // error tracking
    U32 lastErr;

    // posix-like
    U32 tsdUsed;
    int state;
    int cancelState;
    int cancelType;
    int cancelPending;
    void *cancelBuf;
    void *retval;
    U32 joinCount;
    U32 joinableSem;

    // signals
    U32 sigMask;
    U32 sigWaitMask;
    U32 sigPending;
    U32 itimer;
    struct TagListObject sigInfoList;

    // locale
    locale_t locale;

    // stack cfg flag
    U32 stackCfgFlg;
};

// Globals used by the functions under test
extern struct TskModInfo g_tskModInfo;
extern U32 g_tskMaxNum;
extern U32 g_timeSliceCycle;
extern U32 g_systemClock;
extern TskEntryFunc g_tskIdleEntry;
extern TskEntryFunc OsTskIdleBgd;

extern struct TagListObject g_tskCbFreeList;
extern struct TagListObject g_tskRecyleList;

extern struct TagTskCb *g_tskMonList;

// Hooks
typedef void (*ExcTaskInfoFunc)(TskHandle *, struct TskInfo *);
extern void (*g_taskScanHook)(void);
extern ExcTaskInfoFunc g_excTaskInfoGet;
extern U32 (*g_taskNameAdd)(U32, const char *);
extern void (*g_taskNameGet)(U32, char **);
extern void (*g_sysPowerOffHook)(void);

// Other globals
extern U32 g_primaryCoreId;
extern U32 g_numOfCores;
extern U32 OS_TASK_LOCK_DATA;

// TCB table for GET_TCB_HANDLE
#define OS_TSK_NAME_LEN 32
extern struct TagTskCb g_tskCbTable[256];
#define GET_TCB_HANDLE(id) (&g_tskCbTable[(id) % 256])

// Stack cfg flags
#define OS_TSK_STACK_CFG_BY_USER 1u
#define OS_TSK_STACK_CFG_BY_SYS 2u

// Stubs and controllable behavior for tests
extern int stub_OsTskSMPInit_ret;
static inline U32 OsTskSMPInit(void) { return (U32)stub_OsTskSMPInit_ret; }
static inline void OsTaskScan(void) {}
static inline void OsCpuPowerOff(void) {}

extern int stub_alloc_fail;
static inline void *OsMemAllocAlign(U32 mid, U8 pt, U32 size, U32 align)
{
    (void)mid;
    (void)pt;
    (void)align;
    if (stub_alloc_fail)
        return NULL;
    // simple alignment: over-allocate and return aligned address
    uintptr_t raw = (uintptr_t)malloc(size + align + sizeof(void *));
    if (!raw)
        return NULL;
    uintptr_t aligned = (raw + align) & ~((uintptr_t)align - 1u);
    // store original pointer just before aligned for potential free (unused)
    ((void **)aligned)[-1] = (void *)raw;
    return (void *)aligned;
}

extern int stub_OsIdleTskSMPCreate_ret;
static inline U32 OsIdleTskSMPCreate(void) { return (U32)stub_OsIdleTskSMPCreate_ret; }

extern U32 stub_hw_thread_id;
static inline U32 OsGetHwThreadId(void) { return stub_hw_thread_id; }

extern int stub_wakeup_secondary_called;
static inline void OsSmpWakeUpSecondaryCore(void) { stub_wakeup_secondary_called++; }

extern int stub_first_time_switch_called;
static inline void OsFirstTimeSwitch(void) { stub_first_time_switch_called++; }

extern struct TagTskCb *stub_current_tcb;
static inline struct TagTskCb *OsGetCurrentTcb(void) { return stub_current_tcb; }

extern int stub_int_lock_count;
extern int stub_int_restore_count;
static inline uintptr_t OsIntLock(void)
{
    stub_int_lock_count++;
    return 0x1234u;
}
static inline void OsIntRestore(uintptr_t x)
{
    (void)x;
    stub_int_restore_count++;
}

extern struct TagTskCb *stub_task_exit_arg;
static inline void OsTaskExit(struct TagTskCb *tcb) { stub_task_exit_arg = tcb; }

// POSIX/task info stubs
extern int stub_TaskSelf_ret;
extern TskHandle stub_TaskSelf_value;
static inline U32 PRT_TaskSelf(TskHandle *out)
{
    if (out)
        *out = stub_TaskSelf_value;
    return (U32)stub_TaskSelf_ret;
}
extern int stub_TaskGetInfo_called;
extern TskHandle stub_TaskGetInfo_last_id;
static inline U32 PRT_TaskGetInfo(TskHandle id, struct TskInfo *info)
{
    (void)info;
    stub_TaskGetInfo_called++;
    stub_TaskGetInfo_last_id = id;
    return OS_OK;
}

// Misc helpers
static inline int OsCheckStackAddrOverflow(uintptr_t addr, U32 size)
{
    return ((UINTPTR_MAX - addr) < size) ? 1 : 0;
}

// Create/Get TCB for creation
extern int stub_GetTcb_ret;
extern struct TagTskCb *stub_GetTcb_ptr;
static inline U32 OsTaskCreateChkAndGetTcb(struct TagTskCb **out, bool isIdle)
{
    (void)isIdle;
    if (stub_GetTcb_ret == (int)OS_OK)
    {
        if (!stub_GetTcb_ptr)
        {
            stub_GetTcb_ptr = (struct TagTskCb *)calloc(1, sizeof(struct TagTskCb));
            stub_GetTcb_ptr->taskPid = 123;
        }
        *out = stub_GetTcb_ptr;
    }
    return (U32)stub_GetTcb_ret;
}

static inline void OsTskResRecycle(struct TagTskCb *tcb) { (void)tcb; }

static inline void *OsTskContextInit(U32 taskId, U32 stackSize, void *topStack, uintptr_t entry)
{
    (void)taskId;
    (void)entry;
    // Return a plausible stack pointer (topStack + stackSize - 16)
    return (void *)((uintptr_t)topStack + stackSize - 16u);
}

static inline void OsTskSMPTcbInit(struct TskInitParam *p, struct TagTskCb *t)
{
    (void)p;
    (void)t;
}

extern int stub_spin_lock_count;
extern int stub_spin_unlock_count;
static inline void OsSpinLockTaskRq(struct TagTskCb *t)
{
    (void)t;
    stub_spin_lock_count++;
}
static inline void OsSpinUnlockTaskRq(struct TagTskCb *t)
{
    (void)t;
    stub_spin_unlock_count++;
}

#define TSK_CREATE_DEL_LOCK() \
    do                        \
    {                         \
    } while (0)
#define TSK_CREATE_DEL_UNLOCK() \
    do                          \
    {                           \
    } while (0)

// Name add/get hooks default to NULL; tests can set them
// Trace hook
static inline void sys_trace_task_create(U32 id, struct TskInitParam *p)
{
    (void)id;
    (void)p;
}

// Task scan hook type
typedef void (*TaskScanHookType)(void);

// Helper test runner utilities
static inline int run_check(const char *caseName, long long expected, long long actual)
{
    int pass = (expected == actual);
    printf("[CASE] %s\n", caseName);
    printf("  Expected: %lld\n", expected);
    printf("  Actual  : %lld\n", actual);
    printf("  Result  : %s\n\n", pass ? "PASS" : "FAIL");
    return pass;
}

// Global instances/variables definitions for tests (declare in one TU usually). Each test C can define TEST_UTILS_DEFINE_GLOBALS to instantiate.
#ifdef TEST_UTILS_DEFINE_GLOBALS
U32 UNI_FLAG = 0;
void *libc_global_locale = NULL;
struct TskModInfo g_tskModInfo = {0};
U32 g_tskMaxNum = 0;
U32 g_timeSliceCycle = 0;
U32 g_systemClock = 1000000u; // 1 MHz default for tests
TskEntryFunc g_tskIdleEntry = NULL;
TskEntryFunc OsTskIdleBgd = (TskEntryFunc)0x1; // non-NULL marker
struct TagListObject g_tskCbFreeList = LIST_OBJECT_INIT(g_tskCbFreeList);
struct TagListObject g_tskRecyleList = LIST_OBJECT_INIT(g_tskRecyleList);
struct TagTskCb *g_tskMonList = NULL;
void (*g_taskScanHook)(void) = NULL;
ExcTaskInfoFunc g_excTaskInfoGet = NULL;
U32 (*g_taskNameAdd)(U32, const char *) = NULL;
void (*g_taskNameGet)(U32, char **) = NULL;
void (*g_sysPowerOffHook)(void) = NULL;
U32 g_primaryCoreId = 0;
U32 g_numOfCores = 1;
U32 OS_TASK_LOCK_DATA = 1;
struct TagTskCb g_tskCbTable[256] = {0};

int stub_OsTskSMPInit_ret = OS_OK;
int stub_alloc_fail = 0;
int stub_OsIdleTskSMPCreate_ret = OS_OK;
U32 stub_hw_thread_id = 0;
int stub_wakeup_secondary_called = 0;
int stub_first_time_switch_called = 0;
struct TagTskCb *stub_current_tcb = NULL;
int stub_int_lock_count = 0;
int stub_int_restore_count = 0;
struct TagTskCb *stub_task_exit_arg = NULL;
int stub_TaskSelf_ret = OS_OK;
TskHandle stub_TaskSelf_value = 0;
int stub_TaskGetInfo_called = 0;
TskHandle stub_TaskGetInfo_last_id = 0;
int stub_GetTcb_ret = OS_OK;
struct TagTskCb *stub_GetTcb_ptr = NULL;
int stub_spin_lock_count = 0;
int stub_spin_unlock_count = 0;
#endif // TEST_UTILS_DEFINE_GLOBALS

#endif // TEST_UTILS_H
