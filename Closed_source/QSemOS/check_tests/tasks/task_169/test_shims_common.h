#ifndef TEST_SHIMS_COMMON_H
#define TEST_SHIMS_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* Basic typedefs to match expected types */
    typedef uint32_t U32;
    typedef uint32_t TimerHandle;

    typedef U32 (*StartTimerFunc)(TimerHandle);
    typedef U32 (*StopTimerFunc)(TimerHandle);
    typedef U32 (*TimerQueryFunc)(TimerHandle, U32 *);

    typedef struct TimerApi
    {
        StartTimerFunc startTimer;
        StopTimerFunc stopTimer;
        TimerQueryFunc timerQuery;
    } TimerApi;

    /* Externs implemented in each test C file */
    extern TimerApi g_timerApi[];

/* Timer type and handle helpers */
/* For testing, pack type in low 4 bits to easily craft values */
#define TIMER_TYPE_INVALID 4u /* allow 0..3 as valid types; 4 is invalid for tests */
#define OS_TIMER_TYPE_MASK 0xFu
#define OS_TIMER_GET_TYPE(handle) ((U32)((handle) & OS_TIMER_TYPE_MASK))

    static inline TimerHandle make_handle(U32 type, U32 id)
    {
        return (TimerHandle)(((id & 0x0FFFFFFFu) << 4) | (type & OS_TIMER_TYPE_MASK));
    }

#ifdef __cplusplus
}
#endif

#endif /* TEST_SHIMS_COMMON_H */
