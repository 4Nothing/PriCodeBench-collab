kernel_pid_t thread_create(char *stack, int stacksize, uint8_t priority,
                           int flags, thread_task_func_t function, void *arg,
                           const char *name)
{
    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

#ifdef DEVELHELP
    int total_stacksize = stacksize;
#endif
#ifndef CONFIG_THREAD_NAMES
    (void)name;
#endif

    /* align the stack on a 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t)stack % alignof(void *);
    if (misalignment) {
        misalignment = alignof(void *) - misalignment;
        stack += misalignment;
        stacksize -= misalignment;
    }

    /* make room for the thread control block */
    stacksize -= sizeof(thread_t);

    /* round down the stacksize to a multiple of thread_t alignments (usually 16/32bit) */
    stacksize -= stacksize % alignof(thread_t);

    if (stacksize < 0) {
        DEBUG("thread_create: stacksize is too small!\n");
        return -EINVAL;
    }
    /* allocate our thread control block at the top of our stackspace. Cast to
     * (uintptr_t) intermediately to silence -Wcast-align. (We manually made
     * sure alignment is correct above.) */
    thread_t *thread = (thread_t *)(uintptr_t)(stack + stacksize);

#ifdef PICOLIBC_TLS
#if __PICOLIBC_MAJOR__ > 1 || __PICOLIBC_MINOR__ >= 8
#define TLS_ALIGN       (alignof(thread_t) > _tls_align() ? alignof(thread_t) : _tls_align())
#else
#define TLS_ALIGN       alignof(thread_t)
#endif
    char *tls = stack + stacksize - _tls_size();
    /*
     * Make sure the TLS area is aligned as required and that the
     * resulting stack will also be aligned as required
     */
    thread->tls = (void *) ((uintptr_t) tls & ~ (TLS_ALIGN - 1));
    stacksize = (char *) thread->tls - stack;

    _init_tls(thread->tls);
#endif

#if defined(DEVELHELP) || defined(SCHED_TEST_STACK) \
    || defined(MODULE_TEST_UTILS_PRINT_STACK_USAGE)
    if (flags & THREAD_CREATE_NO_STACKTEST) {
        /* create stack guard. Alignment has been handled above, so silence
         * -Wcast-align */
        *(uintptr_t *)(uintptr_t)stack = (uintptr_t)stack;
    }
    else {
        /* assign each int of the stack the value of it's address. Alignment
         * has been handled above, so silence -Wcast-align */
        uintptr_t *stackmax = (uintptr_t *)(uintptr_t)(stack + stacksize);
        uintptr_t *stackp = (uintptr_t *)(uintptr_t)stack;

        while (stackp < stackmax) {
            *stackp = (uintptr_t)stackp;
            stackp++;
        }
    }
#endif

    unsigned state = irq_disable();

    kernel_pid_t pid = KERNEL_PID_UNDEF;
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i) {
        if (sched_threads[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid == KERNEL_PID_UNDEF) {
        DEBUG("thread_create(): too many threads!\n");

        irq_restore(state);

        return -EOVERFLOW;
    }

    sched_threads[pid] = thread;

    thread->pid = pid;
    thread->sp = thread_stack_init(function, arg, stack, stacksize);

#if defined(DEVELHELP) || IS_ACTIVE(SCHED_TEST_STACK) || \
    defined(MODULE_MPU_STACK_GUARD) || defined(MODULE_CORTEXM_STACK_LIMIT)
    thread->stack_start = stack;
#endif

#ifdef DEVELHELP
    thread->stack_size = total_stacksize;
#endif
#ifdef CONFIG_THREAD_NAMES
    thread->name = name;
#endif

    thread->priority = priority;
    thread->status = STATUS_STOPPED;

    thread->rq_entry.next = NULL;

#ifdef MODULE_CORE_MSG
    thread->wait_data = NULL;
    thread->msg_waiters.next = NULL;
    cib_init(&(thread->msg_queue), 0);
    thread->msg_array = NULL;
#endif

    sched_num_threads++;

    DEBUG("Created thread %s. PID: %" PRIkernel_pid ". Priority: %u.\n", name,
          thread->pid, priority);

    if (flags & THREAD_CREATE_SLEEPING) {
        sched_set_status(thread, STATUS_SLEEPING);
    }
    else {
        sched_set_status(thread, STATUS_PENDING);

        if (!(flags & THREAD_CREATE_WOUT_YIELD)) {
            irq_restore(state);
            sched_switch(priority);
            return pid;
        }
    }

    irq_restore(state);

    return pid;
}
