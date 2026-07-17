static inline void unicoap_message_payload_set(unicoap_message_t* message, uint8_t* payload,
                                               size_t size)
{
    message->payload_representation = UNICOAP_PAYLOAD_CONTIGUOUS;
    message->payload = payload;
    message->payload_size = size;
}