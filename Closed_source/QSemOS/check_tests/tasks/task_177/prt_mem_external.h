#ifndef MOCK_PRT_MEM_EXTERNAL_H
#define MOCK_PRT_MEM_EXTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "prt_queue_external.h"

#define OS_MID_QUEUE 0U
#define OS_MEM_DEFAULT_FSC_PT 0U

    /* Malloc/Free hooks controllable by tests - defined in notifier_test_stubs.c */
    extern int g_mock_alloc_fail;
    extern void *g_mock_last_alloc;

    void *OsMemAlloc(U32 mid, U32 flags, U32 size);
    void OsMemFree(void *p);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_PRT_MEM_EXTERNAL_H */
