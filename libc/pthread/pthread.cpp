#include "pthread.hh"
#include <limits.h>
#include <task.h>
#include <errno.h>
#include <stdio.h>

static ptr_val_t __tsd_key_mutex = 0;

#define PTHREAD_STUBBED() \
    fprintf(stderr, "%s: Not implemented!\n", __PRETTY_FUNCTION__);

const unsigned tsd_nkeys = PTHREAD_KEYS_MAX;
__thread void *__tsd[tsd_nkeys] = {0};
__thread pthread_t current_pthread = 0;
bool __tsd_used[tsd_nkeys] = {false};

typedef void (*__pthread_dtor)(void*);
__pthread_dtor  __tsd_dtor[tsd_nkeys] = {0};


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine) (void *), void *arg)
{
    (void)thread;
    (void)attr;
    (void)start_routine;
    (void)arg;
    //TODO
    return -1;
}

int pthread_join(pthread_t thread, void** retval)
{
    (void)thread;
    (void)retval;
    return -1;
}

int pthread_key_create(pthread_key_t* key, void (*dtor)(void*))
{
    LockMutex mtx(&__tsd_key_mutex);

    bool ok = false;
    unsigned int i = 0;
    for (i = 0; i < tsd_nkeys; ++i) {
        if (!__tsd_used[i]) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        return ENOMEM;
    }

    __tsd_used[i] = true;
    *key = i;
    __tsd_dtor[i] = dtor;

    return 0;
}

int pthread_atfork(void (*prepare)(void), void (*parent)(void),
    void (*child)(void))
{
    (void)prepare;
    (void)parent;
    (void)child;
    return 0;
}

extern "C" int register_atfork(void (*prepare)(void), void (*parent)(void),
    void (*child)(void), void *__dso_handle)
{
    (void)prepare;
    (void)parent;
    (void)child;
    (void)__dso_handle;
    return 0;
}

int pthread_key_delete(pthread_key_t key)
{
    (void)key;
    PTHREAD_STUBBED();
    return EINVAL;
}

void* pthread_getspecific(pthread_key_t key)
{
    // FIXME
    return __tsd[key];
}

int pthread_setspecific(pthread_key_t key, const void* value)
{
    __tsd[key] = const_cast<void*>(value);
    return 0;
}

#include <gdt.h>
extern "C" pthread_t pthread_self()
{
/*
    printf("the pthread_self1\n");
    printf("the pthread_self2: %x\n", get_gs_base());
    printf("the pthread_self3: %x\n", &current_pthread);
*/
    return current_pthread;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    (void)attr;
    if (scope == PTHREAD_SCOPE_PROCESS) {
        return ENOTSUP;
    }
    return 0;
}

int pthread_attr_getscope(pthread_attr_t *attr, int *scope)
{
    (void)attr;
    *scope = PTHREAD_SCOPE_SYSTEM;
    return 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    (void)state;
    (void)oldstate;
    PTHREAD_STUBBED();
    return 0;
}

int pthread_setcanceltype(int state, int *oldstate)
{
    (void)state;
    (void)oldstate;
    PTHREAD_STUBBED();
    return 0;
}

void pthread_testcancel(void)
{
    PTHREAD_STUBBED();
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    attr->__attr = PTHREAD_MUTEX_DEFAULT;
    return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    (void)attr;
    //FIXME
    //new (attr) thread_attr;
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    (void)attr;
/*
    auto *a = from_libc(attr);
    if (a != nullptr && a->cpuset != nullptr) {
        delete a->cpuset;
    }
*/
    return 0;
}

extern "C" int pthread_yield()
{
    //TODO
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    (void)attr;
    (void)stacksize;
    //from_libc(attr)->stack_size = stacksize;
    return 0;
}

int pthread_detach(pthread_t thread)
{
    (void)thread;
/*
    pthread* p = pthread::from_libc(thread);
    p->_thread.detach();
*/
    return 0;
}

void pthread_exit(void *retval)
{
    (void)retval;
/*
    auto t = pthread::from_libc(current_pthread);
    t->_retval = retval;
    t->_thread.exit();
*/
    //FIXME
    while(1) {}
}

int pthread_kill(pthread_t thread, int sig)
{
    (void)thread;
    (void)sig;
    PTHREAD_STUBBED();
    return EINVAL;
}

int pthread_sigmask(int how, const sigset_t* set, sigset_t* oldset)
{
    (void)how;
    (void)set;
    (void)oldset;
    return 0;
    //return sigprocmask(how, set, oldset);
}

// From MUSL
/*
void _pthread_cleanup_push(struct __ptcb *cb, void (*f)(void *), void *x)
{
    cb->__f = f;
    cb->__x = x;
    __do_cleanup_push(cb);
}

void _pthread_cleanup_pop(struct __ptcb *cb, int run)
{
    __do_cleanup_pop(cb);
    if (run) cb->__f(cb->__x);
}

*/
