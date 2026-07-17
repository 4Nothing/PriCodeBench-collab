static int OsPerfTaskFilter(U32 taskId)
{
    U32 i;

    if (!g_perfCb.taskIdsNr) {
        return TRUE;
    }

    for (i = 0; i < g_perfCb.taskIdsNr; i++) {
        if (g_perfCb.taskIds[i] == taskId) {
            return TRUE;
        }
    }

    return FALSE;
}
