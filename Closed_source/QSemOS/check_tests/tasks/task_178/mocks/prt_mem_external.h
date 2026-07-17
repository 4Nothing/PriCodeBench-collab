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

    /* Malloc/Free hooks controllable by tests */
    static int g_mock_alloc_fail = 0; /* when set non-zero, next allocation returns NULL */
    static void *g_mock_last_alloc = NULL;

    static inline void *OsMemAlloc(U32 mid, U32 flags, U32 size)
    {
        (void)mid;
        (void)flags;
        if (g_mock_alloc_fail)
        {
            g_mock_alloc_fail = 0; /* one-shot failure */
            return NULL;
        }
        void *p = malloc(size);
        g_mock_last_alloc = p;
        return p;
    }

    static inline void OsMemFree(void *p)
    {
        if (p)
            free(p);
    }

#ifdef __cplusplus
}
#endif

#endif /* MOCK_PRT_MEM_EXTERNAL_H */
