static inline bool uuid_equal(const uuid_t *uuid1, const uuid_t *uuid2)
{
    return (memcmp(uuid1, uuid2, sizeof(uuid_t)) == 0);
}
