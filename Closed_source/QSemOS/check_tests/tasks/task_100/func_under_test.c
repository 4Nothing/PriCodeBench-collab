OS_SEC_L4_TEXT void OsHwiDefaultHandler(HwiArg arg)
{
    g_defHandlerHwiNum = OS_IRQ2HWI(arg);
    OS_REPORT_ERROR(OS_ERRNO_HWI_UNCREATED);
}
