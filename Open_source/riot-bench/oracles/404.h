static inline void bhp_msg_claim_thread(bhp_msg_t *bhp_msg, kernel_pid_t pid)
{
    bhp_msg->pid = pid;
}
