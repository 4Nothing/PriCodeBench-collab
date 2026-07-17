#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 3;
struct TagSemCb semPool[3];
struct TagSemCb *g_allSem = semPool;
struct TagListObject g_unusedSemList;
struct TagTskCb g_runningTask = {.taskPid = 5};

// Variables referenced by func_under_test.c
static uintptr_t g_semPrioLock = 0;

// Stubs
U32 OsSemInitCb(void) {
    OS_LIST_INIT(&g_unusedSemList);
    for (U32 i = 0; i < g_maxSem; i++) {
        g_allSem[i].semId = (U16)i;
        OS_LIST_INIT(&g_allSem[i].semList);
        ListTailAdd(&g_allSem[i].semList, &g_unusedSemList);
    }
    return OS_OK;
}

#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;

    // Case 1: OsSemInit calls OsSemInitCb
    U32 r1 = OsSemInit();
    print_case_u32("OsSemInit calls InitCb", OS_OK, r1, &pass, &total);

    // Case 2: Verify free list populated
    bool nonEmpty = !ListEmpty(&g_unusedSemList);
    print_case_bool("Free list populated", true, nonEmpty, &pass, &total);

    // Case 3: Verify sem IDs assigned
    bool idsOk = (g_allSem[0].semId == 0) && (g_allSem[1].semId == 1) && (g_allSem[2].semId == 2);
    print_case_bool("Sem IDs assigned", true, idsOk, &pass, &total);

    // Case 4: Re-init should succeed
    U32 r4 = OsSemInit();
    print_case_u32("Re-OsSemInit OK", OS_OK, r4, &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
