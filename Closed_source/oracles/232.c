static void OsDynModuleSetError(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (g_dynModuleErrorStr == NULL) {
        g_dynModuleErrorStr = (char *)PRT_MemAlloc(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, OS_MODULE_ERROR_STR_LEN);
        if (g_dynModuleErrorStr == NULL) {
            va_end(ap);
            return;
        }
    }
    (void)memset_s(g_dynModuleErrorStr, OS_MODULE_ERROR_STR_LEN, 0, OS_MODULE_ERROR_STR_LEN);
    int ret = vsnprintf_s(g_dynModuleErrorStr, OS_MODULE_ERROR_STR_LEN, OS_MODULE_ERROR_STR_LEN, fmt, ap);
    if (ret == -1) {
        (void)PRT_MemFree(OS_MID_DYNAMIC, g_dynModuleErrorStr);
        g_dynModuleErrorStr = NULL;
	    va_end(ap);
        return;
    }
	va_end(ap);
}
