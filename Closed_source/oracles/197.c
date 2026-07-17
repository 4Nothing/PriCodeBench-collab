OS_SEC_L4_TEXT locale_t *PRT_LocaleCurrent(void)
{
    struct TagTskCb *tskCb = RUNNING_TASK;

    if (!tskCb) {
        return NULL;
    }

    if (!tskCb->locale) {
        tskCb->locale = (locale_t)libc_global_locale;
    }

    return &tskCb->locale;
}
