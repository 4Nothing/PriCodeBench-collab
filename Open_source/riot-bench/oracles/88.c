int credman_load_private_key(const void *buf, size_t buf_len, credman_credential_t *cred)
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
     * From https://tools.ietf.org/html/rfc5958#section-2
     *
     * OneAsymmetricKey ::= SEQUENCE {
     *      version                   Version,
     *      privateKeyAlgorithm       PrivateKeyAlgorithmIdentifier,
     *      privateKey                PrivateKey,
     *      attributes            [0] Attributes OPTIONAL,
     *      ...,
     *      [[2: publicKey        [1] PublicKey OPTIONAL ]],
     *      ...
     * }
     *
     * PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
     *                                   { PUBLIC-KEY,
     *                                     { PrivateKeyAlgorithms } }
     *
     * AlgorithmIdentifier   ::=  SEQUENCE  {
     *      algorithm               OBJECT IDENTIFIER,
     *      parameters              ANY DEFINED BY algorithm OPTIONAL  }
     *
     * PrivateKey ::= OCTET STRING
     *                   -- Content varies based on type of key.  The
     *                   -- algorithm identifier dictates the format of
     *                   -- the key.
     */

    /* the outer container is a 'OneAsymmetricKey', which should be a SEQUENCE */
    if (priv_key.type != ASN1_TYPE_SEQUENCE) {
        DEBUG("credman: the private key information should be contained in an ASN.1 SEQUENCE\n");
        return CREDMAN_INVALID;
    }

    /* point to version */
    asn1_tree *node = priv_key.child;

    if (!node || node->type != ASN1_TYPE_INTEGER) {
        DEBUG("credman: invalid private key version\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKeyAlgorithm */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_SEQUENCE || !node->length) {
        DEBUG("credman: invalid private key algorithm identifier\n");
        return CREDMAN_INVALID;
    }

    /* for now only ECDSA is supported by credman */
    /* the algorithm should be Elliptic Curve Public Key (OID 1.2.840.10045.2.1) */
    asn1_tree *algorithm = node->child;
    if (sizeof(ecPublicKey) != algorithm->length) {
        DEBUG("credman: wrong private key algorithm, only ecPublicKey is supported\n");
        return CREDMAN_INVALID;
    }

    if (memcmp(ecPublicKey, algorithm->data, sizeof(ecPublicKey)) != 0) {
        DEBUG("credman: wrong OID for algorithm. Expected 1.2.840.10045.2.1\n");
        return CREDMAN_INVALID;
    }

    /* point to privateKey */
    node = node->next;
    if (!node || node->type != ASN1_TYPE_OCTET_STRING || !node->data || !node->length) {
        DEBUG("credman: no private key found\n");
        return CREDMAN_INVALID;
    }

    return credman_load_private_ecc_key(node->data, node->length, cred);

}