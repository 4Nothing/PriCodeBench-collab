void bhp_msg_isr_cb(void *bhp_msg_ctx)
{
    bhp_msg_t *bhp_msg = bhp_msg_ctx;

    assert(bhp_msg->pid != KERNEL_PID_UNDEF);
    msg_send(&bhp_msg->msg, bhp_msg->pid);
}
