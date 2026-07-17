static inline uint8_t* unicoap_message_payload_get(unicoap_message_t* message)
{
    assert(message->payload_representation == UNICOAP_PAYLOAD_CONTIGUOUS);
    return message->payload;
}