static inline void bhp_msg_handler(msg_t *msg)
{
    assert(msg->type == BHP_MSG_BH_REQUEST);
    bhp_irq_handler(msg->content.ptr);
}
