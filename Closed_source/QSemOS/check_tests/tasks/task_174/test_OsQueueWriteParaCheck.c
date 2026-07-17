#include "test_support.h"
#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;
    reset_task();

    /* Case 1: innerId >= g_maxQueue */
    total++;
    U32 r1 = OsQueueWriteParaCheck(100, 1, 1, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("ParaCheck invalid id", OS_ERRNO_QUEUE_INVALID, r1);

    /* Case 2: bufferAddr==0 */
    total++;
    r1 = OsQueueWriteParaCheck(0, 0, 1, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("ParaCheck null buf", OS_ERRNO_QUEUE_PTR_NULL, r1);

    /* Case 3: bufferSize==0 */
    total++;
    r1 = OsQueueWriteParaCheck(0, 1234, 0, OS_QUEUE_NORMAL);
    pass += assert_eq_u32("ParaCheck size zero", OS_ERRNO_QUEUE_SIZE_ZERO, r1);

    /* Case 4: invalid prio */
    total++;
    r1 = OsQueueWriteParaCheck(0, 1234, 1, 99);
    pass += assert_eq_u32("ParaCheck prio invalid", OS_ERRNO_QUEUE_PRIO_INVALID, r1);

    /* Case 5: all good */
    total++;
    r1 = OsQueueWriteParaCheck(0, 1234, 5, OS_QUEUE_URGENT);
    pass += assert_eq_u32("ParaCheck ok", OS_OK, r1);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
