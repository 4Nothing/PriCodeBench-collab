static inline void unicoap_message_payload_set_chunks(unicoap_message_t* message, iolist_t* chunks)
{
    message->payload_representation = UNICOAP_PAYLOAD_NONCONTIGUOUS;
    message->payload_chunks = chunks;
}