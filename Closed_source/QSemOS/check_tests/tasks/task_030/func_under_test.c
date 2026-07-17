OS_SEC_L4_TEXT U32 OsSemCreate(U32 count, U32 semType, enum SemMode semMode,
                               SemHandle *semHandle, U32 cookie)
{
    uintptr_t intSave;
    U32 ret;

    if (semHandle == NULL) {
        return OS_ERRNO_SEM_PTR_NULL;
    }

    SEM_INIT_IRQ_LOCK(intSave);

    ret = OsSemCreateCb(count, semType, semMode, semHandle, cookie);
    if (ret != OS_OK) {
        SEM_INIT_IRQ_UNLOCK(intSave);
        return ret;
    }

    SEM_INIT_IRQ_UNLOCK(intSave);
    return OS_OK;
}
