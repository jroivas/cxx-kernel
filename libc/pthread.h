#ifndef __PTHREAD_H
#define __PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define _POSIX_THREADS

typedef ptr_val_t pthread_cond_t;
typedef ptr_val_t pthread_condattr_t;
typedef ptr_val_t pthread_mutex_t;
typedef ptr_val_t pthread_mutexattr_t;

#define PTHREAD_COND_INITIALIZER 0
#define PTHREAD_MUTEX_INITIALIZER 0

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_destroy(pthread_cond_t *cond);

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);



#ifdef __cplusplus
}
#endif

#endif
