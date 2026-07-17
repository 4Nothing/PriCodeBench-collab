#include "test_support.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    reset_task();

    struct TagQueCb q = {0};
    U8 storage[3 * 32] = {0};
    init_queue_cb(&q, storage, 32, 3);

    /* Case 1: readableCnt > 0 returns OK and decrements */
    total++;
    U16 cnt = 2;
    U32 r1 = OsInnerPend(&cnt, &q.readList, OS_QUEUE_NO_WAIT, &q);
    pass += assert_eq_u32("OsInnerPend immediate", OS_OK, r1);

    /* Case 2: NO_WAIT when count==0 returns NO_SOURCE */
    total++;
    cnt = 0;
    U32 r2 = OsInnerPend(&cnt, &q.readList, OS_QUEUE_NO_WAIT, &q);
    pass += assert_eq_u32("OsInnerPend no source", OS_ERRNO_QUEUE_NO_SOURCE, r2);

    /* Case 3: interrupt active returns IN_INTERRUPT */
    total++;
    cnt = 0;
    g_os_int_active = 1;
    U32 r3 = OsInnerPend(&cnt, &q.readList, 10, &q);
    g_os_int_active = 0;
    pass += assert_eq_u32("OsInnerPend in interrupt", OS_ERRNO_QUEUE_IN_INTERRUPT, r3);

    /* Case 4: task lock returns PEND_IN_LOCK */
    total++;
    cnt = 0;
    OS_TASK_LOCK_DATA = 1;
    U32 r4 = OsInnerPend(&cnt, &q.readList, 10, &q);
    OS_TASK_LOCK_DATA = 0;
    pass += assert_eq_u32("OsInnerPend in lock", OS_ERRNO_QUEUE_PEND_IN_LOCK, r4);

    /* Case 5: timeout path - count==0, timeout set, returns TIMEOUT */
    total++;
    cnt = 0;
    U32 r5 = OsInnerPend(&cnt, &q.readList, 5, &q);
    pass += assert_eq_u32("OsInnerPend timeout", OS_ERRNO_QUEUE_TIMEOUT, r5);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
