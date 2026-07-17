int OsRegisterDieNotifier(struct NotifierBlock *nb)
{
    return OsNotifierChainRegister(&g_dieNotifier, nb);
}
