void bhp_msg_init(bhp_msg_t *bhp_msg, bhp_cb_t cb, void *ctx)
{
    bhp_set_cb(&bhp_msg->bhp, cb, ctx);
    bhp_msg->pid = KERNEL_PID_UNDEF;
    bhp_msg->msg.type = BHP_MSG_BH_REQUEST;
    bhp_msg->msg.content.ptr = &bhp_msg->bhp;
}
