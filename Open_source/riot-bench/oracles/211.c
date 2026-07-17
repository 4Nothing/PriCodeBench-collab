void isrpipe_init(isrpipe_t *isrpipe, uint8_t *buf, size_t bufsize)
{
    isrpipe->mutex = (mutex_t)MUTEX_INIT_LOCKED;
    tsrb_init(&isrpipe->tsrb, buf, bufsize);
}