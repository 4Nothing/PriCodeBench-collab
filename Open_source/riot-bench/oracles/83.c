int credman_add(const credman_credential_t *credential)
{
    credman_credential_t *entry = NULL;
    assert(credential);
    mutex_lock(&_mutex);
    int pos = -1;
    int ret = CREDMAN_ERROR;

    if ((credential->type == CREDMAN_TYPE_EMPTY) ||
        (credential->tag == CREDMAN_TAG_EMPTY)) {
        DEBUG("credman: invalid credential type/tag\n");
        ret = CREDMAN_INVALID;
        goto end;
    }
    switch (credential->type) {
    case CREDMAN_TYPE_PSK:
        if ((credential->params.psk.key.s == NULL) ||
            (credential->params.psk.key.len == 0)) {
            DEBUG("credman: invalid PSK parameters\n");
            ret = CREDMAN_INVALID;
            goto end;
        }
        break;
    case CREDMAN_TYPE_ECDSA:
        if ((credential->params.ecdsa.private_key == NULL) ||
            (credential->params.ecdsa.public_key.x == NULL) ||
            (credential->params.ecdsa.public_key.y == NULL)) {
            DEBUG("credman: invalid ECDSA parameters\n");
            ret = CREDMAN_INVALID;
            goto end;
        }
        break;
    default:
        ret = CREDMAN_TYPE_UNKNOWN;
        goto end;
    }

    pos = _find_credential_pos(credential->tag, credential->type, &entry);
    if (pos >= 0) {
        DEBUG("credman: credential with tag %d and type %d already exist\n",
              credential->tag, credential->type);
        ret = CREDMAN_EXIST;
    }
    else if (entry == NULL) {
        DEBUG("credman: no space for new credential\n");
        ret = CREDMAN_NO_SPACE;
    }
    else {
        *entry = *credential;
        used++;
        ret = CREDMAN_OK;
    }
end:
    mutex_unlock(&_mutex);
    return ret;
}