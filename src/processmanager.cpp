#include "processmanager.h"
#include "arch/platform.h"
#include "kernel.h"
#include "mutex.hh"

#define SCHEDULING_GRANULARITY (KERNEL_FREQUENCY >> 1)
#define SCHEDULING_GRANULARITY_MIN (KERNEL_FREQUENCY / 50)

static ptr_val_t __pm_mutex = 0;

ProcessManager::ProcessManager()
{
    for (uint32_t i = 0; i < TASK_POOLS; i++) {
        m_task_pool[i] = new List();
    }
    m_tasks = new List();
    m_pool_index = 0;
    m_pool_target = 0;
    m_current = NULL;
    m_running = false;
    m_pid = 1;

    m.assign(&__pm_mutex);
}

uint32_t ProcessManager::approxPool(Task *t, uint32_t base)
{
    if (t == NULL) {
        return TASK_POOLS - 1;
    }

    uint32_t approx = (MAX_PRIORITY - t->priority()) + t->nice();
    approx = approx * TASK_POOLS / (MAX_PRIORITY + MAX_NICE + 1);

    approx = (m_pool_index + approx + base) % TASK_POOLS;
    return approx;
}

void ProcessManager::addTask(Task *t)
{
    if (t == NULL) return;

    Platform::seizeInterrupts();

    Task *current = m_current;
    Platform::video()->printf("CURR %d %x\n", m_tasks->size(), current);

    if (current != NULL) {
        current->lock();
    }

    if (t->pid() == 0) {
        t->setPid(++m_pid);
    }

    volatile ptr_val_t *lock = NULL;
    if (current != NULL) {
        lock = current->getLock();
        m_tasks->append(current);
        if (current->save()) {
            Platform::continueInterrupts();
            return;
        }
    }

    m_current = t;

    t->switchTo(lock, (ptr_t)&ProcessManager::killer);
}

void ProcessManager::doKill()
{
    if (m_current == NULL) return;
    m_tasks->deleteAll(m_current);
}

void ProcessManager::killer()
{
    ProcessManager *pm = Platform::processManager();
    pm->doKill();
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
    Task *current = m_current;

    if (current != NULL) {
        //current->lock();
        TEST_LOCK(current);
    }
    //Platform::video()->printf("Curr %x %d\n",current, current==NULL?0:current->pid());
    //Platform::video()->printf("Curr %d\n", m_tasks->size());

    void *tmp = m_tasks->takeFirst();
    if (tmp == NULL) {
        current->unlock();
        Platform::continueInterrupts();
        return NULL;
    }

    Task *next = (Task*)tmp;
    if (next == current) {
        current->unlock();
        Platform::continueInterrupts();
        return current;
    }
    if (current != NULL) {
        m_tasks->append(current);
    }

    m_current = next;

    if (*(next->getLock()) == 1) {
        Platform::video()->printf("NEXT ALREADY LOCKED: %d %x!\n",next->pid(), next);
        while(1) ;
    }

    //next->lock();
    TEST_LOCK(next);

    volatile ptr_val_t *lock = NULL;
    if (current != NULL) {
        lock = current->getLock();
    }

    if (current->save()) {
        next->unlock();
        current->unlock();
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
        if (m_pool_slice < SCHEDULING_GRANULARITY_MIN) {
            m_pool_slice = SCHEDULING_GRANULARITY_MIN;
        }
    } else {
        m_pool_slice = SCHEDULING_GRANULARITY / approx;
    }

    next->setSlice(m_pool_slice);
#else
    next->setSlice(SCHEDULING_GRANULARITY);
#endif

#ifdef DEBUG
    Platform::video()->printf("\n::: %s slice: %5d,  %d\n",next->name(), m_pool_slice, m_tasks->size());
    Platform::video()->printf("\n:::%d\n",next->pid());
#endif

    //Platform::continueInterrupts();
    next->unlock();

    next->restore(lock);

    return current;
}

Task *ProcessManager::clone()
{
    return NULL;
}
