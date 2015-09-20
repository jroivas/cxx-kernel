#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine) (void *), void *arg);
int pthread_join(pthread_t thread, void** retval);
int pthread_kill(pthread_t thread, int sig);

int pthread_key_create(pthread_key_t* key, void (*dtor)(void*));
int pthread_key_delete(pthread_key_t key);
void* pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void* value);

int pthread_atfork(void (*prepare)(void), void (*parent)(void),
    void (*child)(void));
int register_atfork(void (*prepare)(void), void (*parent)(void),
    void (*child)(void), void *__dso_handle);

pthread_t pthread_self();

int pthread_sigmask(int how, const sigset_t* set, sigset_t* oldset);
int pthread_setcancelstate(int state, int *oldstate);

//struct pthread *real__pthread_self();
//struct pthread *__pthread_self();

#ifdef __cplusplus
}
#endif
