OS_SEC_L0_TEXT uintptr_t PRT_SplIrqLock(struct PrtSpinLock *spinLock)
{
    (void)spinLock;
    return PRT_IntLock();
}