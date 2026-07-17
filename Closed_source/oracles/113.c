void OsPerfStop(void)
{
    U32 ret;
    U32 cpuid = PRT_GetCoreID();

    if (g_pmu == NULL) {
        printf("when stop perf, pmu not registered!\n");
        return;
    }

    if (g_perfCb.pmuStatusPerCpu[cpuid] != PERF_PMU_STOPED) {
        ret = g_pmu->stop();
        if (ret != OS_OK) {
            printf("perf stop on core:%u failed, ret = 0x%x\n", cpuid, ret);
            return;
        }

        if (!g_perfCb.needStoreToBuffer) {
            OsPerfPrintCount();
        }

        g_perfCb.pmuStatusPerCpu[cpuid] = PERF_PMU_STOPED;
    } else {
        printf("core:%u status err 0x%x\n", cpuid, g_perfCb.pmuStatusPerCpu[cpuid]);
    }

    return;
}
