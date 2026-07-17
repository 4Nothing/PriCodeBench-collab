OS_SEC_L4_TEXT U32 PRT_SplLockInit(struct PrtSpinLock *spinLock)
{
    if(spinLock == NULL) {
        return OS_ERRNO_SPL_ALLOC_ADDR_INVALID;
    }
    return OsSplLockInit(spinLock);
}
