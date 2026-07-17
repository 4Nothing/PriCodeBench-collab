#include <stdio.h>
#include <string.h>
#include "auto_stub.h"
#include "../task_090/func_under_test.c"
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
    char fifo[10];
    for (int i = 0; i < 10; ++i)
        fifo[i] = (char)(i + 1);
    Ringbuf rb;
    memset(&rb, 0, sizeof(rb));
    rb.fifo = fifo;
    rb.size = sizeof(fifo);

    /* Prepare wrap: start near end, end at small index meaning wrapped data */
    rb.startIdx = 8;
    rb.endIdx = 3; /* available right = 2 (8..9) */
    rb.remain = 5; /* arbitrary valid remain; function computes cpSize by right & size */
    char out[10];

    /* Case 1: read right portion then linear remainder (total 5) */
    total++;
    equals_u32("Case 1 (total 5 with wrap)", 5, OsRingbufReadLoop(&rb, out, 5), &pass);

    /* Case 2: after wrap, startIdx should be 0 */
    total++;
    equals_u32("Case 2 (startIdx after wrap)", 3, rb.startIdx, &pass);

    /* Case 3: then linear can read remaining from 0..endIdx */
    rb.startIdx = 0;
    rb.endIdx = 5;
    rb.remain = rb.size - 5;
    total++;
    equals_u32("Case 3 (read 4 of 5)", 4, OsRingbufReadLoop(&rb, out, 4), &pass);

    /* Case 4: zero-size */
    total++;
    equals_u32("Case 4 (size 0)", 0, OsRingbufReadLoop(&rb, out, 0), &pass);

    /* Case 5: only right-side data available (no linear remainder) */
    rb.startIdx = 9;
    rb.endIdx = 0;
    rb.remain = rb.size - 1; /* only 1 byte used at index 9 */
    total++;
    equals_u32("Case 5 (right-only=1)", 1, OsRingbufReadLoop(&rb, out, 5), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}
