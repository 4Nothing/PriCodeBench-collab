#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "common.h"
#include <stdio.h>

/* Ensure inline functions in prt_queue.c have internal linkage for tests */
#ifdef OS_SEC_ALW_INLINE
#undef OS_SEC_ALW_INLINE
#endif
#ifdef INLINE
#undef INLINE
#endif
#define OS_SEC_ALW_INLINE static
#define INLINE inline

/* Override OS_QUEUE_INNER_ID to treat queueId as direct array index */
/* The real code uses (queueId - 1), but tests expect direct indexing */
#undef OS_QUEUE_INNER_ID
#define OS_QUEUE_INNER_ID(queueId) (queueId)

/* Queue globals - defined in notifier_test_stubs.c */
#define MAX_TEST_QUEUES 4
extern U16 g_maxQueue;
extern struct TagQueCb *g_allQueue;

/* Global state - defined in notifier_test_stubs.c */
extern volatile int g_os_int_active;
extern volatile int g_os_hwi_active;
extern volatile U32 OS_TASK_LOCK_DATA;
extern struct TagTskCb *g_running_task;

/* Mock allocation failure flags */
extern int g_mock_alloc_fail;
extern int g_mock_memset_fail;

/* Task functions - defined in notifier_test_stubs.c */
void OsTskReadyDel(struct TagTskCb *t);
void OsTskReadyAddBgd(struct TagTskCb *t);
void OsTskTimerAdd(struct TagTskCb *t, U32 ticks);
void OsTskSchedule(void);
void OsIntRestore(uintptr_t save);

/* Memory functions */
void *OsMemAlloc(U32 mid, U32 flags, U32 size);
void OsMemFree(void *ptr);

/* Helper to reset global running task state */
extern void reset_task(void);

#endif /* TEST_SUPPORT_H */
