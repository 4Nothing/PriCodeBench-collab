OS_SEC_L4_TEXT void OsTskInfoGet(TskHandle *threadId, struct TskInfo *taskInfo)
{
    U32 ret;

    if ((g_tskMaxNum > 0) && ((UNI_FLAG & OS_FLG_BGD_ACTIVE) != 0)) { /* 任务存在时 */
        ret = PRT_TaskSelf(threadId);
        if (ret == OS_OK) {
            /* 获取当前任务信息 */
            OS_ERR_RECORD(PRT_TaskGetInfo((*threadId), taskInfo));
        }
    }
}
