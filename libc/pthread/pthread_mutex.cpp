#include "pthread.hh"
#include <mutex.hh>
#include <errno.h>

Mutex* __from_libc(pthread_mutex_t* m)
{
    return reinterpret_cast<Mutex*>(m);
}

int pthread_mutex_init(pthread_mutex_t* __restrict m,
        const pthread_mutexattr_t* __restrict attr)
{
    //FIXME: respect attr
    (void)attr;
    new ((ptr_t)m) Mutex;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *m)
{
    __from_libc(m)->~Mutex();
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *m)
{
    __from_libc(m)->lock();
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *m)
{
    __from_libc(m)->unlock();
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *m)
{
    if (__from_libc(m)->isLocked()) {
        return -EBUSY;
    }
    __from_libc(m)->lock();
    return 0;
}
