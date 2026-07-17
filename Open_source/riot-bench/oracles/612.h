static inline iolist_t* unicoap_message_payload_get_chunks(unicoap_message_t* message)
{
    assert(message->payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS);
    return message->payload_chunks;
}