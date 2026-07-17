int credman_get(credman_credential_t *credential, credman_tag_t tag,
                credman_type_t type)
{
    assert(credential);
    mutex_lock(&_mutex);
    int ret = CREDMAN_ERROR;

    int pos = _find_credential_pos(tag, type, NULL);
    if (pos < 0) {
        DEBUG("credman: credential with tag %d and type %d not found\n",
              tag, type);
        ret = CREDMAN_NOT_FOUND;
    }
    else {
        memcpy(credential, &credentials[pos], sizeof(credman_credential_t));
        ret = CREDMAN_OK;
    }
    mutex_unlock(&_mutex);
    return ret;
}