static inline ssize_t unicoap_pdu_build_rfc7252(uint8_t* pdu, size_t capacity,
                                                const unicoap_message_t* message,
                                                const unicoap_message_properties_t* properties)
{
    ssize_t res = 0;

    if ((res = unicoap_pdu_build_header_rfc7252(pdu, capacity, message, properties)) < 0) {
        return res;
    }
    return unicoap_pdu_build_options_and_payload(pdu + res, capacity - res, message) + res;
}