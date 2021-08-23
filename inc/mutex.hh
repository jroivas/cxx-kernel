#ifndef MUTEX_H
#define MUTEX_H

class Mutex;

#include "types.h"
#include "atomic.hh"
#include "uart.hh"
#include "waitlist.hh"
#include "scheduling.hh"

#if __cplusplus >= 201103L
#define STATE_DELETE = delete
#else
#define STATE_DELETE {}
#endif


class Spinlock
{
public:
    Spinlock() : locked(0) { }
    ~Spinlock() { }

    void lock() {
        while (!__sync_bool_compare_and_swap(&locked, 0, 1))
            yield();
        __sync_synchronize();
    }
    bool trylock() {
        if  (!__sync_bool_compare_and_swap(&locked, 0, 1))
            return false;
        __sync_synchronize();
        return true;
    }
    void unlock() {
        __sync_synchronize();
        locked = 0;
    }
    bool isLocked() const {
        return locked;
    }
private:
    volatile ptr_val_t locked;
};

class BaseMutex
{
public:
    BaseMutex() : value(0) {}
    void lock()
    {
        locker.lock();
        if (!value) {
            value = 1;
            locker.unlock();
        } else {
            queue.enqueue();
            locker.unlock();
            reschedule();
        }
    }
    void unlock()
    {
        locker.lock();
        if (queue.empty()) {
            value = 0;
        } else {
            queue.dequeue();
        }
        locker.unlock();
    }
    volatile ptr_val_t *getLock()
    {
        return &value;
    }
    bool isLocked() const {
        return value;
    }

protected:
    volatile ptr_val_t value;
    mutable Spinlock locker;
    Waitlist queue;
};

class Mutex : public BaseMutex
{
public:
    Mutex() : BaseMutex() {}
};

class GuardMutex : public Mutex
{
public:
    GuardMutex(volatile ptr_val_t *ptr) : guard(ptr)
    {
        if (guard == nullptr)
            guard = &value;
    }
    void lock() {
        locker.lock();
        if (!*guard) {
            *guard = 1;
            locker.unlock();
        } else {
            queue.enqueue();
            locker.unlock();
            reschedule();
        }
    }
    void unlock() {
        locker.lock();
        if (queue.empty()) {
            *guard = 0;
        } else {
            queue.dequeue();
        }
        locker.unlock();
    }
    void abort() {
        // Not implemented
    }
private:
    volatile ptr_val_t *guard;
};

class MutexLocker
{
public:
    MutexLocker(BaseMutex* m) : m_mutex(m) {
        m_mutex->lock();
    }
    ~MutexLocker() {
        if (m_mutex->isLocked())
            m_mutex->unlock();
    }

private:
    BaseMutex *m_mutex;
};

#endif
