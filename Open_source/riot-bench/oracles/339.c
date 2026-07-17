bool unicoap_message_payload_is_empty(const unicoap_message_t* message)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS:
        return iolist_is_empty(message->payload_chunks);
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        return message->payload_size == 0;
    default:
        UNREACHABLE();
        return -1;
    }
}