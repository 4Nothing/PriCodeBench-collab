/* Use module-provided stubs and our test helpers */
#include <stdio.h>
#include <stdlib.h>
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

    struct TagSemCb s;
    int pass = 0, total = 0;

    /* Case 1: sem UNUSED => INVALID */
    test_init_sem(&s, OS_SEM_UNUSED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    run_case("UNUSED sem", OS_ERRNO_SEM_INVALID, OsSemPostErrorCheck(&s, 0), &pass, &total);

    /* Case 2: count >= MAX => OVERFLOW */
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), OS_SEM_COUNT_MAX, SEM_MODE_FIFO);
    run_case("count at MAX", OS_ERRNO_SEM_OVERFLOW, OsSemPostErrorCheck(&s, 0), &pass, &total);

    /* Case 3: count just below MAX => OK */
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), OS_SEM_COUNT_MAX - 1, SEM_MODE_FIFO);
    run_case("count below MAX", OS_OK, OsSemPostErrorCheck(&s, 0), &pass, &total);

    /* Case 4: count zero => OK */
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);
    run_case("count zero", OS_OK, OsSemPostErrorCheck(&s, 0), &pass, &total);

    /* Case 5: arbitrary OK */
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 5, SEM_MODE_FIFO);
    run_case("count mid", OS_OK, OsSemPostErrorCheck(&s, 0), &pass, &total);

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return (pass == total) ? 0 : 1;
}
