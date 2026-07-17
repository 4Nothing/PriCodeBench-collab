int thread_kill_zombie(kernel_pid_t pid)
{
    DEBUG("thread_kill: Trying to kill PID %" PRIkernel_pid "...\n", pid);
    unsigned state = irq_disable();

    int result = (int)STATUS_NOT_FOUND;

    thread_t *thread = thread_get(pid);

    if (!thread) {
        DEBUG("thread_kill: Thread does not exist!\n");
    }
    else if (thread->status == STATUS_ZOMBIE) {
        DEBUG("thread_kill: Thread is a zombie.\n");

        sched_threads[pid] = NULL;
        sched_num_threads--;
        sched_set_status(thread, STATUS_STOPPED);

        result =  1;
    }
    else {
        DEBUG("thread_kill: Thread is not a zombie!\n");
    }
    irq_restore(state);
    return result;
}
