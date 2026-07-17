void* OsDynModuleLoad(const char *moduleFile, S32 mode)
{
    U32 ret;
    char *moduleStr = NULL;
    Elf_Ehdr *elfInfo = NULL;
    struct stat moduleFileSata;
    struct DynModuleUnitInfo *unitInfo = NULL;
    if (moduleFile == NULL) {
        OsDynModuleSetError("The file path is empty.");
        return NULL;
    }

    ret = OsDynModuleFileCheck(moduleFile, &moduleFileSata, &unitInfo);
    if (ret != OS_MODULE_OK) {
        OsDynModuleSetError("File check failed.");
        return NULL;
    }

    if (unitInfo != NULL) {
        if (unitInfo->state == MODULE_UNIT_INIT) {
            OsDynModuleSetError("The module is already loading, please try again later.");
            return NULL;
        }
        return unitInfo;
    }

    ret = OsDynModuleGetFileContent(moduleFile, &moduleStr);
    if ((ret != OS_MODULE_OK) || (moduleStr == NULL)) {
        OsDynModuleSetError("Get file content failed, ret:%u.", ret);
        return NULL;
    }

    elfInfo = (Elf_Ehdr *)moduleStr;
    ret = OsDynModuleCheckElf(elfInfo);
    if (ret != 0) {
        OsDynModuleSetError("ELF check failed, ret:%u.", ret);
        (void)PRT_MemFree(OS_MID_DYNAMIC, moduleStr);
        return NULL;
    }

    ret = OsDynModuleUnitAlloc(&unitInfo);
    if (ret != 0) {
        OsDynModuleSetError("Alloc memory failed for the dynamic module unit, ret:%u.", ret);
        (void)PRT_MemFree(OS_MID_DYNAMIC, moduleStr);
        return NULL;
    }
    unitInfo->moduleStr = moduleStr;
    unitInfo->stDev = moduleFileSata.st_dev;
    unitInfo->stIno = moduleFileSata.st_ino;
    unitInfo->state = MODULE_UNIT_INIT;

    ret = OsDynModuleUnitLoad(unitInfo, mode);
    if (ret != 0) {
        OsDynModuleSetError("Load the dynamic module unit failed, ret:%u.", ret);
        OsDynModuleUnitFree(unitInfo);
        return NULL;
    }

    (void)PRT_MemFree(OS_MID_DYNAMIC, unitInfo->moduleStr);
    unitInfo->moduleStr = NULL;
    unitInfo->state = MODULE_UNIT_ACTIVE;
    return (void *)unitInfo;
}
