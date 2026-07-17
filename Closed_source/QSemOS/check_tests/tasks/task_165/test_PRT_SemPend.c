#include <stdio.h>
#include "test_helpers.h"
#include "os_stub.h"
#include "func_under_test.c"

static void run_case(const char *name, U32 expected, U32 actual, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected=%u, actual=%u\n", name, expected, actual);
    if (expected == actual)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;
    U32 r;

    // Prepare sem 0 as used count with count>0 to take fast path
    test_init_sem(GET_SEM(0), OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 1, SEM_MODE_FIFO);

    // 1) invalid handle
    r = PRT_SemPend(100, 1);
    run_case("invalid handle", OS_ERRNO_SEM_INVALID, r, &pass, &total);

    // 2) interrupt active -> INTERR
    g_int_active = 1;
    r = PRT_SemPend(0, 1);
    run_case("interrupt active", OS_ERRNO_SEM_PEND_INTERR, r, &pass, &total);
    g_int_active = 0;

    // 3) fast acquire path returns OK and decrements count
    r = PRT_SemPend(0, 1);
    run_case("fast acquire", OS_OK, r, &pass, &total);

    // 4) timeout==0 -> UNAVAILABLE
    r = PRT_SemPend(0, 0);
    run_case("timeout zero unavailable", OS_ERRNO_SEM_UNAVAILABLE, r, &pass, &total);

    // 5) finite timeout path: sem count 0 so will pend and then schedule and timeout
    test_init_sem(GET_SEM(0), OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    r = PRT_SemPend(0, 10);
    run_case("finite timeout returns timeout", OS_ERRNO_SEM_TIMEOUT, r, &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}
