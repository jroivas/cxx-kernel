#include "local.h"
#include "types.h"
#include "mutex.hh"

using namespace __cxxabiv1;

int __cxa_guard_acquire(__guard *g)
{
    if (g == nullptr) return 0;

    GuardMutex m((unsigned long volatile*)g);
    m.lock();

    return 1;
}

void __cxa_guard_release (__guard *g)
{
    if (g == nullptr) return;

    GuardMutex m((unsigned long volatile*)g);
    m.unlock();
}

void __cxa_guard_abort(__guard *g)
{
    if (g == nullptr) return;

    GuardMutex m((unsigned long volatile*)g);
    m.abort();
}
