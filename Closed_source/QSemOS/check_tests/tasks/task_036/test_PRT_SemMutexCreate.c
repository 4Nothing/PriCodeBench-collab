#include "support.h"
#include "prt_sem_external.h"

U32 g_maxSem = 3;
struct TagSemCb semPool[3];
struct TagSemCb *g_allSem = semPool;
struct TagListObject g_unusedSemList;
struct TagTskCb g_runningTask = {.taskPid = 5};

// Forward declarations
void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, struct TagSemCb *semCreated);
U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie);
U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie);

#include "func_under_test.c"

// Implement OsSemCreateCbInit
void OsSemCreateCbInit(U32 count, U32 semType, enum SemMode semMode, struct TagSemCb *semCreated) {
    semCreated->semCount = count;
    semCreated->semMode = semMode;
    semCreated->semType = semType;
    OS_LIST_INIT(&semCreated->semList);
    OS_LIST_INIT(&semCreated->semBList);
    semCreated->semOwner = OS_INVALID_OWNER_ID;
    semCreated->semStat = OS_SEM_USED;
    semCreated->recurCount = 0;
}

// Implement OsSemCreate (called by PRT_SemMutexCreate)
U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) {
    (void)count; (void)semType; (void)semMode; (void)cookie;
    if (semHandle == NULL) return OS_ERRNO_SEM_PTR_NULL;
    if (ListEmpty(&g_unusedSemList)) return OS_ERRNO_SEM_ALL_BUSY;
    struct TagListObject *unusedSem = OS_LIST_FIRST(&g_unusedSemList);
    ListDelete(unusedSem);
    struct TagSemCb *semCreated = GET_SEM_LIST(unusedSem);
    OsSemCreateCbInit(count, semType, semMode, semCreated);
    *semHandle = (SemHandle)semCreated->semId;
    return OS_OK;
}

// Implement OsSemCreateCb (called by PRT_SemMutexCreate via OsSemCreate)
U32 OsSemCreateCb(U32 count, U32 semType, enum SemMode semMode, SemHandle *semHandle, U32 cookie) {
    (void)count; (void)semType; (void)semMode; (void)cookie;
    if (ListEmpty(&g_unusedSemList)) return OS_ERRNO_SEM_ALL_BUSY;
    struct TagListObject *unusedSem = OS_LIST_FIRST(&g_unusedSemList);
    ListDelete(unusedSem);
    struct TagSemCb *semCreated = GET_SEM_LIST(unusedSem);
    OsSemCreateCbInit(count, semType, semMode, semCreated);
    *semHandle = (SemHandle)semCreated->semId;
    return OS_OK;
}

static void reset_free_list(void)
{
    OS_LIST_INIT(&g_unusedSemList);
    for (U32 i = 0; i < g_maxSem; i++)
    {
        g_allSem[i].semId = (U16)i;
        OS_LIST_INIT(&g_allSem[i].semList);
        ListTailAdd(&g_allSem[i].semList, &g_unusedSemList);
    }
}

int main(void)
{
    int pass = 0, total = 0;
    reset_free_list();

    // Case 1: Create mutex
    SemHandle h1;
    U32 r1 = PRT_SemMutexCreate(&h1);
    print_case_u32("PRT_SemMutexCreate OK", OS_OK, r1, &pass, &total);

    // Case 2: Created is binary sem
    struct TagSemCb *c1 = GET_SEM(h1);
    bool ok2 = (GET_SEM_TYPE(c1->semType) == SEM_TYPE_BIN);
    print_case_bool("Binary sem created", true, ok2, &pass, &total);

    // Case 3: Initial count is 1 (full)
    print_case_bool("Initial count 1", true, (c1->semCount == 1), &pass, &total);

    // Case 4: Mode is PRIOR
    print_case_bool("Mode PRIOR", true, (c1->semMode == SEM_MODE_PRIOR), &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
