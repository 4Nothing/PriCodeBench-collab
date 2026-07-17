#define RINGBUF_STUBS_DISABLED
#include <stdio.h>
#include <string.h>
#include "auto_stub.h"

static inline uintptr_t PRT_SplIrqLock(PrtSpinLock *lock) { (void)lock; return 0; }
static inline void PRT_SplIrqUnlock(PrtSpinLock *lock, uintptr_t flags) { (void)lock; (void)flags; }
static inline void OsSpinLockInitInner(uintptr_t *lock) { (void)lock; }
#include "../task_090/func_under_test.c"
#include "../task_091/func_under_test.c"
#include "../task_092/func_under_test.c"
#include "../task_093/func_under_test.c"
#include "../task_094/func_under_test.c"
#include "../task_098/func_under_test.c"
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
    char storage[8];
    Ringbuf rb;

    /* Init */
    (void)PRT_RingbufInit(&rb, storage, sizeof(storage));

    /* Case 1: read from empty -> 0 */
    char r1[16];
    total++;
    equals_u32("Case 1 (empty)", 0, PRT_RingbufRead(&rb, r1, sizeof(r1)), &pass);

    /* Write 5 */
    char w1[] = {1, 2, 3, 4, 5};
    (void)PRT_RingbufWrite(&rb, w1, sizeof(w1));

    /* Case 2: read 3 -> 3 */
    total++;
    equals_u32("Case 2 (read 3)", 3, PRT_RingbufRead(&rb, r1, 3), &pass);

    /* Case 3: read remaining 2 -> 2 */
    total++;
    equals_u32("Case 3 (read 2)", 2, PRT_RingbufRead(&rb, r1, 4), &pass);

    /* Case 4: wrap: write 7, read 6 */
    char w2[] = {6, 7, 8, 9, 10, 11, 12};
    (void)PRT_RingbufWrite(&rb, w2, sizeof(w2));
    total++;
    equals_u32("Case 4 (read 6)", 6, PRT_RingbufRead(&rb, r1, 6), &pass);

    /* Case 5: now only 1 left */
    total++;
    equals_u32("Case 5 (read last 1)", 1, PRT_RingbufRead(&rb, r1, 10), &pass);

    double rate = (total == 0) ? 0.0 : (100.0 * pass / total);
    printf("Pass-Rate: %.2f%%\n", rate);
    return (pass == total) ? 0 : 1;
}
