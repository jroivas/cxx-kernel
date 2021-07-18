#include "syscall_proc.h"
#include <platform.h>
#include <errno.h>

long syscall_rt_sigprocmask(int how, const sigset_t *set,
                           sigset_t *oldset, size_t sigsetsize)
{
    (void)how;
    (void)set;
    (void)oldset;
    (void)sigsetsize;

    Platform::video()->printf("rt_sigprocmask: %d, 0x%x, 0x%x, %d\n", how, set, oldset, sigsetsize);

    return -EINVAL;
}

long syscall_gettid(void)
{
    long tid = Platform::processManager()->pid();

    Platform::video()->printf("tid: %u\n", tid);
    return tid;
}

long syscall_tkill(int tid, int sig)
{
    Platform::video()->printf("tkill: %u %d\n", tid, sig);

    return -EPERM;
}

int syscall_sigaltstack(const stack_t *ss, stack_t *old_ss)
{
    (void)ss;
    (void)old_ss;
    Platform::video()->printf("sigaltstack: 0x%lx, 0x%lx\n", ss, old_ss);

    return -EPERM;
}

int syscall_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize)
{
    (void)signum;
    (void)act;
    (void)oldact;

    Platform::video()->printf("rt_sigaction: %d, 0x%x, 0x%x, %d\n", signum, act, oldact, sigsetsize);

    if (sigsetsize != sizeof(sigset_t))
        return -EINVAL;

    /* TODO implement */
    return -EINVAL;
}
