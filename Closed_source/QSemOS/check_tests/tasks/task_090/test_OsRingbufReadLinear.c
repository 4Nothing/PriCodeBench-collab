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
    char fifo[12];
    for (int i = 0; i < 12; ++i)
        fifo[i] = (char)i;
    Ringbuf rb;
    memset(&rb, 0, sizeof(rb));
    rb.fifo = fifo;
    rb.size = sizeof(fifo);
    rb.startIdx = 2;
    rb.endIdx = 9; /* 7 bytes available */
    rb.remain = rb.size - (rb.endIdx - rb.startIdx);
    char out[12];

    /* Case 1: read 0 -> 0 */
    total++;
    equals_u32("Case 1 (size 0)", 0, OsRingbufReadLinear(&rb, out, 0), &pass);

    /* Case 2: read 3 -> 3 */
    total++;
    equals_u32("Case 2 (read 3)", 3, OsRingbufReadLinear(&rb, out, 3), &pass);

    /* Case 3: read 10 but only 4 left (since 3 consumed), initially 7 */
    total++;
    equals_u32("Case 3 (cap by remain=4)", 4, OsRingbufReadLinear(&rb, out, 10), &pass);

    /* Case 4: now nothing left -> 0 */
    total++;
    equals_u32("Case 4 (empty)", 0, OsRingbufReadLinear(&rb, out, 1), &pass);

    /* Case 5: re-prepare and read exact */
    rb.startIdx = 0;
    rb.endIdx = 5;
    rb.remain = rb.size - 5;
    total++;
    equals_u32("Case 5 (exact 5)", 5, OsRingbufReadLinear(&rb, out, 5), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}
