void OsDynModuleUnload(void *handle)
{
    struct DynModuleUnitInfo *unitInfo = (struct DynModuleUnitInfo *)handle;
    if (unitInfo == NULL) {
        return;
    }
    OsDynModuleUnitFree(unitInfo);
}
