int base64_decode(const void *base64_in, size_t base64_in_size,
                  void *data_out, size_t *data_out_size)
{
    uint8_t *out = data_out;
    const uint8_t *in = base64_in;
    size_t required_size = base64_estimate_decode_size(base64_in_size);

    if (in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (*data_out_size < required_size) {
        *data_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (data_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    const uint8_t *end = in + base64_in_size;
    uint8_t decode_buf[4];

    while (1) {
        size_t decode_buf_fill = 0;
        /* Try to load 4 codes into the decode buffer, skipping invalid symbols
         * (such as inserted newlines commonly used to improve readability) */
        do {
            /* Reached end of input before 4 codes were loaded, handle each
             * possible decode buffer fill level individually: */
            if (in == end) {
                switch (decode_buf_fill) {
                    case 0:
                        /* no data in decode buffer -->nothing to do */
                        break;
                    case 1:
                        /* an input size of 4 * n + 1 cannot happen, (even when
                         * dropping the "=" chars) */
                        return BASE64_ERROR_DATA_IN_SIZE;
                    case 2:
                        /* Got two base64 chars, or one byte of output data.
                         * The just fill with zero codes and ignore the two
                         * additionally decoded bytes */
                        decode_buf[2] = decode_buf[3] = 0;
                        decode_four_codes(out, decode_buf);
                        out += 1;
                        break;
                    case 3:
                        /* Got three base64 chars or 2 bytes of output data.
                         * Again, just fill with zero bytes and ignore the
                         * additionally decoded byte */
                        decode_buf[3] = 0;
                        decode_four_codes(out, decode_buf);
                        out += 2;
                        break;
                }
                *data_out_size = (uintptr_t)out - (uintptr_t)data_out;
                return BASE64_SUCCESS;
            }
            switch (decode_buf[decode_buf_fill] = getcode(*in++)) {
                case BASE64_NOT_DEFINED:
                case BASE64_EQUALS:
                    continue;
            }
            decode_buf_fill++;
        }
        while (decode_buf_fill < 4);

        decode_four_codes(out, decode_buf);
        out += 3;
    }
}
