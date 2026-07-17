int credman_load_public_key(const void *buf, size_t buf_len, ecdsa_public_key_t *out)
{
    asn1_tree objects[CONFIG_CREDMAN_MAX_ASN1_OBJ];
    asn1_tree pub_key;

    assert(buf);
    assert(out);

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

    int32_t res = der_decode((uint8_t *)buf, buf_len, &pub_key, objects, obj_count);
    if (res < 0) {
        DEBUG("credman: could not parse the key (%" PRId32 ")\n", res);
        return CREDMAN_INVALID;
    }

    /*
     * From https://tools.ietf.org/html/rfc5280#section-4.1
     *
     * SubjectPublicKeyInfo  ::=  SEQUENCE  {
     *      algorithm               AlgorithmIdentifier,
     *      subjectPublicKey        BIT STRING  }
     *
     * AlgorithmIdentifier   ::=  SEQUENCE  {
     *      algorithm               OBJECT IDENTIFIER,
     *      parameters              ANY DEFINED BY algorithm OPTIONAL  }
     */

    /* the outer container is a 'SubjectPublicKeyInfo', which should be a SEQUENCE */
    if (pub_key.type != ASN1_TYPE_SEQUENCE) {
        DEBUG("credman: the public key information should be contained in an ASN.1 SEQUENCE\n");
        return CREDMAN_INVALID;
    }

    asn1_tree *algorithm_id = pub_key.child;
    asn1_tree *algorithm = algorithm_id->child;

    /* for now only ECDSA is supported by credman */
    /* the algorithm should be Elliptic Curve Public Key (OID 1.2.840.10045.2.1) */
    if (sizeof(ecPublicKey) != algorithm->length) {
        DEBUG("credman: wrong OID length for algorithm\n");
        return CREDMAN_INVALID;
    }

    if (memcmp(ecPublicKey, algorithm->data, sizeof(ecPublicKey)) != 0) {
        DEBUG("credman: wrong OID for algorithm. Expected 1.2.840.10045.2.1\n");
        return CREDMAN_INVALID;
    }

    return _parse_ecc_point(algorithm_id->next, &out->x, &out->y);
}