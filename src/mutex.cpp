#include "mutex.hh"
#include "types.h"
#include "arch/platform.h"

Mutex::Mutex()
{
    m_ptr = nullptr;
}

Mutex::Mutex(ptr_val_t volatile *ptr)
{
    m_ptr = ptr;
}

Mutex::~Mutex()
{
    m_ptr = nullptr;
}

void Mutex::assign(ptr_val_t volatile *ptr)
{
    m_ptr = ptr;
}

bool Mutex::isLocked()
{
    if (m_ptr == nullptr) return false;

    return (*m_ptr == 1);
}

bool Mutex::wait()
{
    if (m_ptr == nullptr) return false;

    while (*m_ptr == 1) {
        //Sleep
    }

    return true;
}

void Mutex::abort()
{
    //unlock();
}

LockMutex::LockMutex(ptr_val_t volatile *ptr)
    : Mutex(ptr)
{
    lock();
}

LockMutex::~LockMutex()
{
    if (isLocked()) {
        unlock();
    }
}
