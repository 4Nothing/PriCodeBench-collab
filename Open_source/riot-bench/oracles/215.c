int isrpipe_write(isrpipe_t *isrpipe, const uint8_t *buf, size_t n)
{
    int res = tsrb_add(&isrpipe->tsrb, buf, n);

    mutex_unlock(&isrpipe->mutex);

    return res;
}