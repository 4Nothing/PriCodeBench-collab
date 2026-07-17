OS_SEC_L0_TEXT void OsHwiDispatchTail(void)
{
    U64 irqStartTime = 0;
    if (TICK_NO_RESPOND_CNT > 0) {
        if ((UNI_FLAG & OS_FLG_TICK_ACTIVE) != 0) {
            // OsTskContextLoad， 回到被打断的tick处理现场
            return;
        }
#if defined(OS_OPTION_RR_SCHED) && defined(OS_OPTION_RR_SCHED_IRQ_TIME_DISCOUNT)
        irqStartTime = OsCurCycleGet64();
#endif
        UNI_FLAG |= OS_FLG_TICK_ACTIVE;

        do {
            OsIntEnable();
            // tickISRִ，这里开中断
            g_tickDispatcher();
            OsIntDisable();
            TICK_NO_RESPOND_CNT--;
        } while (TICK_NO_RESPOND_CNT > 0);

        UNI_FLAG &= ~OS_FLG_TICK_ACTIVE;
        OS_IRQ_TIME_RECORD(irqStartTime);
    }
#if defined(OS_OPTION_RR_SCHED)
    OsHwiEndCheckTimeSlice(OsCurCycleGet64());
#endif

    OsMainSchedule();
}
