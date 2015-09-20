#include "pthread.hh"
#include <string.hh>

int pthread_cond_init(pthread_cond_t *__restrict cond,
    const pthread_condattr_t *__restrict attr)
{
    (void)attr;
    Mem::set(cond, 0, sizeof(*cond));
    return 0;
}

int pthread_cond_destroy(pthread_cond_t* cond)
{
    (void)cond;
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    (void)cond;
    //from_libc(cond)->wake_all();
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    (void)cond;
    //from_libc(cond)->wake_one();
    return 0;
}

int pthread_cond_wait(pthread_cond_t *__restrict cond,
       pthread_mutex_t *__restrict mutex)
{
    (void)cond;
    (void)mutex;
    return 0;
    //return from_libc(cond)->wait(from_libc(mutex));
}

int pthread_cond_timedwait(pthread_cond_t *__restrict cond,
                           pthread_mutex_t *__restrict mutex,
                           const struct timespec* __restrict ts)
{
    (void)cond;
    (void)mutex;
    (void)ts;
    return 0;
}
