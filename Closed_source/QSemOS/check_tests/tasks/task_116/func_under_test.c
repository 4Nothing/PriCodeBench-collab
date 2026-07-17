U32 OsPmuInit(void)
{
#ifdef OS_OPTION_PERF_HW_PMU
    if (OsHwPmuInit() != OS_OK) {
        return OS_ERRNO_PERF_HW_INIT_ERROR;
    }
#endif

#ifdef OS_OPTION_PERF_TIMED_PMU
    if (OsTimedPmuInit() != OS_OK) {
        return OS_ERRNO_PERF_TIMED_INIT_ERROR;
    }
#endif

#ifdef OS_OPTION_PERF_SW_PMU
    if (OsSwPmuInit() != OS_OK) {
        return OS_ERRNO_PERF_SW_INIT_ERROR;
    }
#endif

    return OS_OK;
}
