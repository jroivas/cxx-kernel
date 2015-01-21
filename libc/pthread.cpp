#include "pthread.h"
#include "mutex.h"

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr)
{
    if (cond == NULL) return 1;

    (void)cond_attr;
    *cond = 0;

    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    //FIXME
    (void)cond;
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    //FIXME
    (void)cond;
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    //FIXME
    (void)cond;
    (void)mutex;
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    //FIXME
    (void)cond;
    (void)mutex;
    (void)abstime;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    //FIXME
    (void)cond;
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
    if (mutex == 0) return 1;

    (void)mutexattr;
    *mutex = 0;

    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    Mutex mtx(mutex);
    do {
        mtx.lock();
    } while (!mtx.isLocked());
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    Mutex mtx(mutex);
    mtx.lock();
    return mtx.isLocked()?0:1;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    Mutex mtx(mutex);
    mtx.unlock();
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}
