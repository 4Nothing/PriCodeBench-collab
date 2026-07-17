void unicoap_message_payload_append_chunk(unicoap_message_t* message, iolist_t* chunk)
{
    assert(message->payload_representation == UNICOAP_PAYLOAD_NONCONTIGUOUS);
    if (message->payload_chunks) {
        iolist_append(&message->payload_chunks, chunk);
    }
    else {
        message->payload_chunks = chunk;
    }
}