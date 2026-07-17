OS_SEC_L2_TEXT void OsTskIdleBgd(void)
{
    OS_SHOOK_ACTIVATE_PARA0(OS_HOOK_IDLE_PREFIX);

    while (1) {
        OsIdleTaskExe();
    }
    
}
