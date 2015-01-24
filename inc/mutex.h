#ifndef MUTEX_H
#define MUTEX_H

class Mutex;

#include "types.h"
#include "atomic.hh"

class Mutex
{
public:
    Mutex();
    Mutex(ptr_val_t volatile *ptr);
    ~Mutex();

    void assign(ptr_val_t volatile *ptr);
    void lock();
    void unlock();
    bool isLocked();
    bool wait();
    void abort();

private:
    ptr_val_t volatile *m_ptr;
};

inline void Mutex::lock() {
    if (m_ptr == NULL) return;

    Platform_CAS(m_ptr, 0, 1);
}

inline void Mutex::unlock() {
    if (m_ptr == NULL) return;

    // If already unlocked return
    if (*m_ptr == 0) return;

    while ((Platform_CAS(m_ptr, 1, 0))==0) ;
}
#endif
