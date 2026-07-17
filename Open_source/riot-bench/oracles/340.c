ssize_t unicoap_message_payload_make_contiguous(unicoap_message_t* message, uint8_t* buffer,
                                                size_t capacity)
{
    switch (message->payload_representation) {
    case UNICOAP_PAYLOAD_NONCONTIGUOUS: {
        assert(buffer);
        ssize_t res = iolist_to_buffer(message->payload_chunks, buffer, capacity);
        if (res < 0) {
            UNICOAP_DEBUG("buf too small " _UNICOAP_NEED_HAVE "\n",
                          iolist_size(message->payload_chunks), capacity);
            return res;
        }
        message->payload_representation = UNICOAP_PAYLOAD_CONTIGUOUS;
        message->payload = buffer;
        message->payload_size = res;
        return res;
    }
    case UNICOAP_PAYLOAD_CONTIGUOUS:
        return message->payload_size;
    default:
        UNREACHABLE();
        return -1;
    }
}