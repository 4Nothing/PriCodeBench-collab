void* OsDynModuleFind(void *handle, const char *symbol)
{
    struct DynModuleUnitInfo *unitInfo = (struct DynModuleUnitInfo *)handle;
    if (unitInfo == NULL) {
        return NULL;
    }
    U16 i;
    for (i = 0; i < unitInfo->symNum; i++) {
        if (strcmp(unitInfo->symTab[i].name, symbol) == 0) {
            return unitInfo->symTab[i].addr;
        }
    }
    return NULL;
}
