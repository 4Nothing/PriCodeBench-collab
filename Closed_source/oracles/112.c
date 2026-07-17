void OsPerfStart(void)
{
    U32 ret;
    U32 cpuid = PRT_GetCoreID();

    if (g_pmu == NULL) {
        printf("when start perf, pmu not registered!\n");
        return;
    }

    if (g_perfCb.pmuStatusPerCpu[cpuid] != PERF_PMU_STARTED) {
        ret = g_pmu->start();
        if (ret != OS_OK) {
            printf("perf start on core:%u failed, ret = 0x%x\n", cpuid, ret);
            return;
        }

        g_perfCb.pmuStatusPerCpu[cpuid] = PERF_PMU_STARTED;
    } else {
        printf("core:%u status err 0x%x\n", cpuid, g_perfCb.pmuStatusPerCpu[cpuid]);
    }

    return;
}
