void OsPerfUpdateEventCount(Event *event, U32 value)
{
    if (event == NULL) {
        return;
    }

    /* event->count is U64 */
    event->count[PRT_GetCoreID()] += (value & 0xFFFFFFFF);

    return;
}
