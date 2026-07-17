#include <stdio.h>
#include <string.h>
#include "auto_stub.h"
#include "../task_092/func_under_test.c"
#include "func_under_test.c"

static int equals_u32(const char *name, U32 expected, U32 actual, int *passCnt)
{
    printf("%s -> Expected: %u, Actual: %u\n", name, expected, actual);
    if (expected == actual)
    {
        (*passCnt)++;
        return 1;
    }
    return 0;
}

int main(void)
{
    int pass = 0;
    int total = 0;
    char buf[10];
    char src[10];
    memset(src, 0xAA, sizeof(src));
    Ringbuf rb;
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = rb.size;
    rb.fifo = buf;
    rb.endIdx = 0;

    /* Case 1: write 4 */
    total++;
    equals_u32("Case 1 (write 4)", 4, OsRingbufWriteLoop(&rb, src, 4), &pass);

    /* Case 2: write to end boundary then continue linearly (total 5) */
    rb.endIdx = 8;
    rb.remain = 6; /* currently used 4; simulate state */
    total++;
    equals_u32("Case 2 (wrap + linear total)", 5, OsRingbufWriteLoop(&rb, src, 5), &pass);

    /* Case 3: endIdx should now be 3 after wrap + linear */
    total++;
    equals_u32("Case 3 (endIdx after wrap)", 3, rb.endIdx, &pass);

    /* Case 4: reset state and write with exact remain */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.fifo = buf;
    rb.remain = 3;
    rb.endIdx = 0;
    total++;
    equals_u32("Case 4 (exact remain=3)", 3, OsRingbufWriteLoop(&rb, src, 3), &pass);

    /* Case 5: zero-size request */
    total++;
    equals_u32("Case 5 (size 0)", 0, OsRingbufWriteLoop(&rb, src, 0), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}
