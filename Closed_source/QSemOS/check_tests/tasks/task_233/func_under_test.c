static U32 OsDynModuleUnitAlloc(struct DynModuleUnitInfo **dynModuleUnitInfo)
{
    U32 i;
    struct DynModuleUnitInfo *unitInfo = NULL;
    uintptr_t intSave = OsModuleIntLock();

    for (i = 0; i < OS_MAX_MODULE_NUM; i++) {
        if (g_dynModuleInfoPool[i] != NULL) {
            continue;
        }
        unitInfo = (struct DynModuleUnitInfo *)PRT_MemAlloc(OS_MID_DYNAMIC, 
            OS_MEM_DEFAULT_FSC_PT, sizeof(struct DynModuleUnitInfo));
        if (unitInfo == NULL) {
            OsModuleIntUnlock(intSave);
            return OS_MODULE_ERRNO_MEMORY_ALLOC;
        }
        (void)memset_s(unitInfo, sizeof(struct DynModuleUnitInfo), 0, sizeof(struct DynModuleUnitInfo));
        break;
    }

    if (unitInfo == NULL) {
        OsModuleIntUnlock(intSave);
        return OS_MODULE_ERRNO_UNIT_FULL;
    }

    g_dynModuleInfoPool[i] = unitInfo;
    unitInfo->unitNo = i;
    unitInfo->state = MODULE_UNIT_FREE;
    *dynModuleUnitInfo = unitInfo;
    OsModuleIntUnlock(intSave);
    return OS_MODULE_OK;
}
