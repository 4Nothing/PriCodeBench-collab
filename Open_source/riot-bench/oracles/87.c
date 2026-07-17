int credman_load_private_ecc_key(const void *buf, size_t buf_len, credman_credential_t *cred)
{
    asn1_tree objects[CONFIG_CREDMAN_MAX_ASN1_OBJ];
    asn1_tree priv_key;

    assert(buf);
    assert(cred);

    int obj_count = der_object_count((uint8_t *)buf, buf_len);
    if (obj_count <= 0) {
        DEBUG("credman: could not calculate the number of elements within the key\n");
        return CREDMAN_INVALID;
    }

    if (obj_count > CONFIG_CREDMAN_MAX_ASN1_OBJ) {
        DEBUG("credman: not enough ASN.1 objects to decode key.\n");
        DEBUG("credman: current max is %d, and we need %d\n",
              CONFIG_CREDMAN_MAX_ASN1_OBJ, obj_count);
        return CREDMAN_INVALID;
    }

    if (der_decode((uint8_t *)buf, buf_len, &priv_key, objects, obj_count) < 0) {
        DEBUG("credman: could not parse the key\n");
        return CREDMAN_INVALID;
    }

    /*
     * From https://tools.ietf.org/html/rfc5915#section-3
     *
     * ECPrivateKey ::= SEQUENCE {
     *      version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
     *      privateKey     OCTET STRING,
     *      parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
     *      publicKey  [1] BIT STRING OPTIONAL
     * }
     */

    /* point to version, it SHALL be 1 */
    asn1_tree *node = priv_key.child;
    if (!node || node->type != ASN1_TYPE_INTEGER || node->data[0] != 0x01) {
        DEBUG("credman: invalid private key version\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKey */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_OCTET_STRING || !node->data || !node->length) {
        DEBUG("credman: invalid private key\n");
        return CREDMAN_INVALID;
    }

    cred->type = CREDMAN_TYPE_ECDSA;
    cred->params.ecdsa.public_key.x = NULL;
    cred->params.ecdsa.public_key.y = NULL;

    cred->params.ecdsa.private_key = node->data;

    /* try to find a publicKey by tag */
    while (node && node->type != ASN1_CONTEXT_TAG(1)) {
        node = node->next;
    }

    if (!node) {
        return CREDMAN_OK;
    }

    return _parse_ecc_point(node->child, &cred->params.ecdsa.public_key.x,
                            &cred->params.ecdsa.public_key.y);
}