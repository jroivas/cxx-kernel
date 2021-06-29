#pragma once

#include <signal.h>

long syscall_rt_sigprocmask(int how, const sigset_t *set,
                           sigset_t *oldset, size_t sigsetsize);
long syscall_gettid(void);
long syscall_tkill(int tid, int sig);
int syscall_sigaltstack(const stack_t *ss, stack_t *old_ss);
