static inline unsigned uuid_version(const uuid_t *uuid)
{
    uint16_t time_hi_vers = byteorder_ntohs(uuid->time_hi);

    return (time_hi_vers & 0xF000) >> 12;
}
