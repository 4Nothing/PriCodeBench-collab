#include <stdio.h>
#include "os_stub.h"
#include "test_helpers.h"
#define OS_OPTION_BIN_SEM 1
#include "func_under_test.c"

static void run_case(const char *name, U32 expected_flags, U32 actual_flags, U32 expected_timer_add, U32 actual_timer_add, int *pass, int *total)
{
    (*total)++;
    printf("%s => expected flags=%u actual=%u | expected timerAdd=%u actual=%u\n", name, expected_flags, actual_flags, expected_timer_add, actual_timer_add);
    if (expected_flags == actual_flags && expected_timer_add == actual_timer_add)
        (*pass)++;
}

int main(void)
{
    test_reset();
    int pass = 0, total = 0;

    struct TagSemCb s;
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);

    // Case1: timeout forever -> no timerAdd
    U32 before = g_timer_add_count;
    OsSemPendListPut(&s, OS_WAIT_FOREVER);
    run_case("forever timeout", OS_TSK_PEND, RUNNING_TASK->status & (OS_TSK_PEND | OS_TSK_TIMEOUT), 0, g_timer_add_count - before, &pass, &total);

    // Reset
    test_reset();
    test_init_sem(&s, OS_SEM_USED, MAKE_SEM_TYPE(SEM_TYPE_COUNT, 0), 0, SEM_MODE_FIFO);

    // Case2: finite timeout -> timerAdd and TIMEOUT flag set
    before = g_timer_add_count;
    OsSemPendListPut(&s, 100);
    run_case("finite timeout", OS_TSK_PEND | OS_TSK_TIMEOUT, RUNNING_TASK->status & (OS_TSK_PEND | OS_TSK_TIMEOUT), 1, g_timer_add_count - before, &pass, &total);

    // Case3: ensure taskPend set
    int expect_ok = (RUNNING_TASK->taskPend == &s);
    total++;
    printf("taskPend set => expected=1, actual=%d\n", expect_ok);
    if (expect_ok)
        pass++;

    // Case4: FIFO enqueue tail
    int is_tail = (s.semList.prev == &RUNNING_TASK->pendList);
    total++;
    printf("enqueued tail => expected=1, actual=%d\n", is_tail);
    if (is_tail)
        pass++;

    // Case5: ReadyDel called once
    total++;
    printf("readyDel count => expected=1, actual=%u\n", g_ready_del_count);
    if (g_ready_del_count == 1)
        pass++;

    printf("Pass-Rate: %.2f%%\n", total ? (pass * 100.0 / total) : 0.0);
    return pass == total ? 0 : 1;
}
