OS_SEC_ALW_INLINE INLINE void OsTimerDelayCount(U64 cycles)
{
    U64 cur;
    U64 end;

    cur = OsCurCycleGet64();
    end = cur + cycles;

    if (cur < end) {
        while (cur < end) {
            cur = OsCurCycleGet64();
        }
    } else {
        /* 考虑Cycle计数反转的问题，虽然64位的计数需要584年才能反转，但是考虑软件的严密性，需要做反转保护 */
        /* 当前值大于结束值的场景 */
        while (cur > end) {
            cur = OsCurCycleGet64();
        }
        /* 正过来之后追赶的场景 */
        while (cur < end) {
            cur = OsCurCycleGet64();
        }
    }
}