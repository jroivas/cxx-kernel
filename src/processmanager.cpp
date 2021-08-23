#include "processmanager.h"
#include "arch/platform.h"
#include "kernel.h"
#include "mutex.hh"

#define SCHEDULING_GRANULARITY (KERNEL_FREQUENCY >> 1)
#define SCHEDULING_GRANULARITY_MIN (KERNEL_FREQUENCY / 50)

static Spinlock __scheduling_lock;

ProcessManager::ProcessManager()
{
    for (uint32_t i = 0; i < TASK_POOLS; i++) {
        m_task_pool[i] = new List();
    }
    m_tasks = new List();
    m_pool_index = 0;
    m_pool_target = 0;
    m_current = nullptr;
    m_running = false;
    m_pid = 1;
}

uint32_t ProcessManager::approxPool(Task *t, uint32_t base)
{
    if (t == nullptr) {
        return TASK_POOLS - 1;
    }

    uint32_t approx = (MAX_PRIORITY - t->priority()) + t->nice();
    approx = approx * TASK_POOLS / (MAX_PRIORITY + MAX_NICE + 1);

    approx = (m_pool_index + approx + base) % TASK_POOLS;
    return approx;
}

void ProcessManager::addTask(Task *t)
{
    if (t == nullptr) return;
    __scheduling_lock.lock();

    Platform::seizeInterrupts();

    Task *current = m_current;
    //Platform::video()->printf("CURR %d %x\n", m_tasks->size(), current);

    if (current != nullptr) {
        current->lock();
    }

    if (t->pid() == 0) {
        t->setPid(++m_pid);
    }

    m_tasks->append(t);

    volatile ptr_val_t *lock = nullptr;
    if (current != nullptr) {
        lock = current->getLock();
        if (current->save()) {
            __scheduling_lock.unlock();
            Platform::continueInterrupts();
            return;
        }
    }

    m_current = t;

    __scheduling_lock.unlock();
    t->switchTo(lock, (ptr_t)&ProcessManager::killer);
}

void ProcessManager::doKill()
{
    if (m_current == nullptr) return;
    __scheduling_lock.lock();
    /* Stop scheduling this one */
    m_tasks->deleteAll(m_current);
    __scheduling_lock.unlock();
}

void ProcessManager::killer()
{
    __scheduling_lock.lock();
    ProcessManager *pm = Platform::processManager();
    pm->doKill();
    __scheduling_lock.unlock();
}

/*
#define TEST_LOCK(x) bool __test_lock_##x = false;\
do {\
    volatile ptr_val_t *__lock_val = x->getLock();\
    x->lock();\
    if (*__lock_val == 1) {\
        __test_lock_##x = true;\
    }\
} while (!__test_lock_##x);
*/
#define TEST_LOCK(x)\
do {\
    volatile ptr_val_t *__lock_val = x->getLock();\
    x->lock();\
    if (*__lock_val == 1) {\
        break;\
    }\
} while (true);

Task *ProcessManager::schedule()
{
    Platform::seizeInterrupts();
    __scheduling_lock.lock();
    Task *current = m_current;

    if (current != nullptr) {
        TEST_LOCK(current);
    }
    //Platform::video()->printf("Curr %x %u total %u\n", current, current == nullptr ? 0 : current->pid(), m_tasks->size());

    void *tmp = m_tasks->first();
    if (tmp == nullptr) {
        /* This should not happen! */
        current->unlock();
        __scheduling_lock.unlock();
        Platform::continueInterrupts();
        return nullptr;
    }

    Task *next = (Task*)tmp;
    if (next == current) {
        /* Have only one task, continue it */
        current->unlock();
        __scheduling_lock.unlock();
        Platform::continueInterrupts();
        return current;
    }
    /* Move next task last */
    m_tasks->rotateFirstLast();
    /* Check it's not blocked */
    while (next->status() == Task::STATUS_BLOCKED) {
        tmp = m_tasks->first();
        if (tmp == nullptr) {
            /* This should not happen! */
            current->unlock();
            __scheduling_lock.unlock();
            Platform::continueInterrupts();
            return nullptr;
        }
        next = (Task*)tmp;
        if (next == current) {
            /* Have only one task, continue it */
            current->unlock();
            __scheduling_lock.unlock();
            Platform::continueInterrupts();
            return current;
        }
        m_tasks->rotateFirstLast();
    }
    /* switch to it  */
    m_current = next;

    if (*(next->getLock()) == 1) {
        Platform::video()->printf("NEXT ALREADY LOCKED: %d %x!\n", next->pid(), next);
        while(1) ;
    }

    TEST_LOCK(next);

    next->setStatus(Task::STATUS_RUNNING);
    volatile ptr_val_t *lock = nullptr;
    if (current != nullptr) {
        if (current->status() != Task::STATUS_BLOCKED)
            current->setStatus(Task::STATUS_WAITING);
        lock = current->getLock();
    }

    if (current->save()) {
        next->unlock();
        current->unlock();
        __scheduling_lock.unlock();
        Platform::continueInterrupts();
        return current;
    }


#if 1
    uint32_t approx = (MAX_PRIORITY - next->priority()) + next->nice();
    if (approx == 0) {
        approx++;
    }

    if (m_tasks->size() > 0) {
        m_pool_slice = SCHEDULING_GRANULARITY / m_tasks->size() / approx;
    } else {
        m_pool_slice = SCHEDULING_GRANULARITY / approx;
    }
    if (m_pool_slice < SCHEDULING_GRANULARITY_MIN) {
        m_pool_slice = SCHEDULING_GRANULARITY_MIN;
    }

    next->setSlice(m_pool_slice);
#else
    next->setSlice(SCHEDULING_GRANULARITY);
#endif

#if 0
    Platform::video()->printf("::: %s slice: %5d,  %d -> %u\n",next->name(), m_pool_slice, m_tasks->size(), next->pid());
#endif
#ifdef DEBUG
    Platform::video()->printf("\n::: %s slice: %5d,  %d\n",next->name(), m_pool_slice, m_tasks->size());
    Platform::video()->printf("\n:::%d\n", next->pid());
#endif

    next->unlock();
    __scheduling_lock.unlock();

    next->restore(lock);

    return current;
}

void ProcessManager::yield()
{
    Platform::seizeInterrupts();
    __scheduling_lock.lock();
    if (m_current != nullptr) {
        m_current->resetSlice();
    }
    __scheduling_lock.unlock();
    Platform::continueInterrupts();
}

Task *ProcessManager::clone()
{
    return nullptr;
}
