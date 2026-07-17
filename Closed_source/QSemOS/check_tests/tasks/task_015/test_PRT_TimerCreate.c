#include "test_common.h"
#include "func_under_test.c"
 // include implementation directly for standalone build

// Helpers to craft handles with type bits (see OS_TIMER_GET_TYPE)
static inline TimerHandle make_handle(U32 type, U32 id)
{
    return (TimerHandle)((type << 28) | (id & 0x0FFFFFFFu));
}

// Dummy successful implementations
static U32 ok_create(struct TimerCreatePara *para, TimerHandle *out)
{
    // encode type into handle using type requested
    *out = make_handle(para->type == OS_TIMER_HARDWARE ? TIMER_TYPE_HWTMR : TIMER_TYPE_SWTMR, 0x55);
    return 0; // success code arbitrary 0
}

int main(void)
{
    U32 ret;
    TimerHandle h;
    struct TimerCreatePara p;

    // Case 1: Null createPara
    ret = PRT_TimerCreate(NULL, &h);
    print_result("Null createPara", OS_ERRNO_TIMER_INPUT_PTR_NULL, ret);

    // Case 2: Null handle pointer
    p.type = OS_TIMER_SOFTWARE;
    p.mode = OS_TIMER_ONCE;
    p.interval = 10;
    ret = PRT_TimerCreate(&p, NULL);
    print_result("Null handle ptr", OS_ERRNO_TIMER_INPUT_PTR_NULL, ret);

    // Case 3: interval == 0
    p.interval = 0;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Interval zero", OS_ERRNO_TIMER_INTERVAL_INVALID, ret);

    // Case 4: invalid mode
    p.interval = 10;
    p.mode = 999;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Invalid mode", OS_ERRNO_TIMER_MODE_INVALID, ret);

    // Case 5: invalid type
    p.mode = OS_TIMER_ONCE;
    p.type = 999;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Invalid type", OS_ERRNO_TIMER_TYPE_INVALID, ret);

    // Prepare API for success and missing handler tests
    memset(g_timerApi, 0, sizeof(g_timerApi));

    // Case 6: missing create handler
    p.type = OS_TIMER_SOFTWARE;
    p.interval = 5;
    p.mode = OS_TIMER_LOOP;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Missing create handler (SW)", OS_ERRNO_TIMER_NOT_INIT_OR_GROUP_NOT_CREATED, ret);

    // Case 7: success SW
    g_timerApi[TIMER_TYPE_SWTMR].createTimer = ok_create;
    p.type = OS_TIMER_SOFTWARE;
    p.interval = 5;
    p.mode = OS_TIMER_ONCE;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Success SW", 0, ret);

    // Case 8: success HW
    g_timerApi[TIMER_TYPE_HWTMR].createTimer = ok_create;
    p.type = OS_TIMER_HARDWARE;
    p.interval = 8;
    p.mode = OS_TIMER_LOOP;
    ret = PRT_TimerCreate(&p, &h);
    print_result("Success HW", 0, ret);

    return print_summary();
}
