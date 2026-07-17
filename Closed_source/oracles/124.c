OS_SEC_TEXT void OsPowerOffFuncHook(PowerOffFuncT powerOffFunc)
{
    g_sysPowerOffHook = powerOffFunc;
}
