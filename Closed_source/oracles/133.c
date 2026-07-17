static char *OsPerfGetEventName(Event *event)
{
    if (event->eventId == PERF_COUNT_CPU_CLOCK) {
        return "timed";
    } else {
        return "unknown";
    }
}
