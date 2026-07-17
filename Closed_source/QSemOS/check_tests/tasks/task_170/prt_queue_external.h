#pragma once
#include "common.h"

/* Queue helper macros */
#define OS_QUEUE_INNER_ID(queueId) ((queueId) & 0xFFFFU)

/* Queue handle array access */
#define MAX_TEST_QUEUES 4
extern struct TagQueCb g_queue_cbs[MAX_TEST_QUEUES];
extern U32 g_maxQueue;
#define GET_QUEUE_HANDLE(innerId) ((innerId) < g_maxQueue ? &g_queue_cbs[(innerId)] : NULL)

/* List to TCB conversion */
#define GET_TCB_PEND(ptr) LIST_COMPONENT(ptr, struct TagTskCb, pendList)
#define LIST_COMPONENT(ptr, type, field) ((type *)((char *)(ptr) - offsetof(type, field)))