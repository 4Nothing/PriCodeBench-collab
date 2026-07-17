OS_SEC_L2_TEXT void OsIdleTaskExe(void) {
    OsVoidFunc coreSleep;

    OS_MHOOK_ACTIVATE_PARA0(OS_HOOK_IDLE_PERIOD);

    coreSleep = g_taskCoreSleep;
    if (coreSleep != NULL) {
        coreSleep();
    }
}