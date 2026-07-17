static inline size_t unicoap_message_payload_get_size(const unicoap_message_t* message)
{
    return message->payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS ?
               iolist_size(message->payload_chunks) :
               message->payload_size;
}