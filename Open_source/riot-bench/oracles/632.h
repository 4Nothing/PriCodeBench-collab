static inline ssize_t unicoap_pdu_parse_rfc7252_result(uint8_t* pdu, size_t size,
                                                       unicoap_parser_result_t* parsed)
{
    parsed->message.options = &parsed->options;
    return unicoap_pdu_parse_rfc7252(pdu, size, &parsed->message, &parsed->properties);
}