ssize_t unicoap_message_payload_copy(const unicoap_message_t* message, uint8_t* buffer,
                                     size_t capacity)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS:
        return iolist_to_buffer(message->payload_chunks, buffer, capacity);
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        if (message->payload_size > capacity) {
            UNICOAP_DEBUG("buf too small " _UNICOAP_NEED_HAVE "\n",
                          message->payload_size, capacity);
            return -ENOBUFS;
        }
        memcpy(buffer, message->payload, message->payload_size);
        return message->payload_size;
    default:
        UNREACHABLE();
        return -1;
    }
}