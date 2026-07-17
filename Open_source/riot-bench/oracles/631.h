static inline ssize_t unicoap_pdu_buildv_rfc7252(uint8_t* header, size_t header_capacity,
                                                 const unicoap_message_t* message,
                                                 const unicoap_message_properties_t* properties,
                                                 iolist_t iolists[UNICOAP_PDU_IOLIST_COUNT])
{
    ssize_t res = 0;

    if ((res =
         unicoap_pdu_build_header_rfc7252(header, header_capacity, message, properties)) < 0) {
        return res;
    }
    return unicoap_pdu_buildv_options_and_payload(header, res, message, iolists);
}