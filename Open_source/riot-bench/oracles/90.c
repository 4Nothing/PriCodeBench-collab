void credman_reset(void)
{
    mutex_lock(&_mutex);
    memset(credentials, 0,
           sizeof(credman_credential_t) * CONFIG_CREDMAN_MAX_CREDENTIALS);
    used = 0;
    mutex_unlock(&_mutex);
}