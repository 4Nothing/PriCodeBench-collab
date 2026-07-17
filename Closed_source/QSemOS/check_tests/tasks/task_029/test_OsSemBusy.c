#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 4;
struct TagSemCb semPool[4];
struct TagSemCb *g_allSem = semPool;
struct TagTskCb g_runningTask = {.taskPid = 42};

#include "func_under_test.c"

static void setup_sem(U32 idx, U32 type, U32 count, U32 owner)
{
    g_allSem[idx].semType = type;
    g_allSem[idx].semCount = count;
    g_allSem[idx].semOwner = owner;
}

int main(void)
{
    int pass = 0, total = 0;

    // Case 1: binary, count 0 -> busy
    setup_sem(0, SEM_TYPE_BIN, 0, OS_INVALID_OWNER_ID);
    print_case_bool("bin count=0 busy", true, OsSemBusy(0), &pass, &total);

    // Case 2: binary, count 1 -> not busy
    setup_sem(1, SEM_TYPE_BIN, 1, OS_INVALID_OWNER_ID);
    print_case_bool("bin count=1 not busy", false, OsSemBusy(1), &pass, &total);

    // Case 3: recursive mutex, count 0, owner != running -> busy
    setup_sem(2, SEM_TYPE_BIN | (SEM_MUTEX_TYPE_RECUR << 4), 0, 7);
    print_case_bool("recur mutex held by other", true, OsSemBusy(2), &pass, &total);

    // Case 4: recursive mutex, count 0, owner == running -> not busy
    setup_sem(3, SEM_TYPE_BIN | (SEM_MUTEX_TYPE_RECUR << 4), 0, g_runningTask.taskPid);
    print_case_bool("recur mutex held by self", false, OsSemBusy(3), &pass, &total);

    // Case 5: count sem type, count 0 -> not busy per function logic
    setup_sem(0, SEM_TYPE_COUNT, 0, OS_INVALID_OWNER_ID);
    print_case_bool("counting sem not busy", false, OsSemBusy(0), &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
