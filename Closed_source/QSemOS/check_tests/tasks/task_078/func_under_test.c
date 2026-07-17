OS_SEC_L4_TEXT void OsInitSigVectors(struct TagTskCb *taskCb)
{
    for (int i = 0; i < PRT_SIGNAL_MAX; i++) {
        taskCb->sigVectors[i] = OsSigDefaultHandler;
    }
    return;
}
