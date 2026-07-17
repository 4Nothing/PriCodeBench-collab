int OsNotifyDie(int val, void *v)
{
    return NotifierCallChain(&g_dieNotifier.head, val, v, -1, NULL);
}
