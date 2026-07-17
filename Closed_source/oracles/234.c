static void OsDynModuleUnitFree(struct DynModuleUnitInfo *unitInfo)
{
    U32 i;
    if (unitInfo == NULL) {
        return;
    }
    if (unitInfo->moduleStr != NULL) {
        (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo->moduleStr);
        unitInfo->moduleStr = NULL;
    }
    if (unitInfo->symTab != NULL) {
        for (i = 0; i < unitInfo->symNum; i++) {
            if (unitInfo->symTab[i].name != NULL) {
                (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo->symTab[i].name);
                unitInfo->symTab[i].name = NULL;
            }
        }
        (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo->symTab);
        unitInfo->symTab = NULL;
    }
    if (unitInfo->loadSegMem != NULL) {
        (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo->loadSegMem);
        unitInfo->loadSegMem = NULL;
    }
    g_dynModuleInfoPool[unitInfo->unitNo] = NULL;
    (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo);
    unitInfo = NULL;
}
