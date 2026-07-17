void credman_delete(credman_tag_t tag, credman_type_t type)
{
    mutex_lock(&_mutex);
    int pos = _find_credential_pos(tag, type, NULL);
    if (pos >= 0) {
        explicit_bzero(&credentials[pos], sizeof(credman_credential_t));
        used--;
    }
    mutex_unlock(&_mutex);
}