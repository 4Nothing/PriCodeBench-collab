#include <stdio.h>
#include <string.h>
#include "auto_stub.h"
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
    char buf[16];
    char src[16];
    memset(src, 0xAB, sizeof(src));
    Ringbuf rb;

    /* Case 1: remain == 0 -> 0 */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = 0;
    rb.endIdx = 0;
    rb.fifo = buf;
    total++;
    equals_u32("Case 1 (remain 0)", 0, OsRingbufWriteLinear(&rb, src, 5), &pass);

    /* Case 2: normal write 3 */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = 10;
    rb.endIdx = 0;
    rb.fifo = buf;
    total++;
    equals_u32("Case 2 (write 3)", 3, OsRingbufWriteLinear(&rb, src, 3), &pass);

    /* Case 3: request 0 -> 0 */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = 10;
    rb.endIdx = 5;
    rb.fifo = buf;
    total++;
    equals_u32("Case 3 (size 0)", 0, OsRingbufWriteLinear(&rb, src, 0), &pass);

    /* Case 4: remain less than request */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = 2;
    rb.endIdx = 1;
    rb.fifo = buf;
    total++;
    equals_u32("Case 4 (cap by remain=2)", 2, OsRingbufWriteLinear(&rb, src, 5), &pass);

    /* Case 5: sequential writes */
    memset(&rb, 0, sizeof(rb));
    rb.size = sizeof(buf);
    rb.remain = 10;
    rb.endIdx = 0;
    rb.fifo = buf;
    total++;
    U32 r1 = OsRingbufWriteLinear(&rb, src, 3); /* 3 */
    U32 r2 = OsRingbufWriteLinear(&rb, src, 4); /* 4 */
    equals_u32("Case 5 (seq 3 then 4 -> second ret)", 4, r2, &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}
