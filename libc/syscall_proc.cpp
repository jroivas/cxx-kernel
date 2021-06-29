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

    errno = -EINVAL;
    return -1;
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

    errno = -EPERM;
    return -1;
}
