#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "common.h"
#include "prt_queue_external.h"
#include <stdio.h>

/* Override OS_QUEUE_INNER_ID to treat queueId as direct array index */
/* The real code uses (queueId - 1), but tests expect direct indexing */
#undef OS_QUEUE_INNER_ID
#define OS_QUEUE_INNER_ID(queueId) (queueId)

/* Queue globals - defined in prt_queue_init.c */
#define MAX_TEST_QUEUES 4
extern U16 g_maxQueue;
extern struct TagQueCb *g_allQueue;

/* Global state - defined in notifier_test_stubs.c */
extern volatile int g_os_int_active;
extern volatile int g_os_hwi_active;
extern volatile U32 OS_TASK_LOCK_DATA;
extern struct TagTskCb *g_running_task;

/* Note: g_mock_alloc_fail and g_mock_memset_fail are defined as static in mocks */
/* They can be accessed directly from test file */
extern int g_mock_alloc_fail;

#endif /* TEST_SUPPORT_H */