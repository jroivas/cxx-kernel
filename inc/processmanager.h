#ifndef _PROCESS_MANAGER_H
#define _PROCESS_MANAGER_H

#include "task.h"
#include "list.hh"
#include "mutex.hh"

#define TASK_POOLS 4

class ProcessManager
{
public:
    ProcessManager();
    void addTask(Task *t);
    Task *schedule();
    bool isRunning() const
    {
        return m_running;
    }
    void setRunning()
    {
        m_running=true;
    }
    bool hasSlice() const
    {
        if (m_current == NULL) {
            return false;
        }
        return (m_current->slice() > 0);
    }
    void decSlice()
    {
        if (m_current == NULL) return;

        m_current->lock();
        m_current->decSlice();
        m_current->unlock();
    }
    uint32_t pid() const
    {
        if (m_current == NULL) {
            return 0;
        }
        return m_current->pid();
    }
    static void killer();
    Task *clone();

private:
    void doKill();
    uint32_t approxPool(Task *t, uint32_t base=0);
    bool m_running;
    List *m_tasks;
/*
    List *m_tasks_hi;
    List *m_tasks_lo;
*/
    List *m_task_pool[TASK_POOLS];
    uint32_t m_pool_index;
    uint32_t m_pool_target;
    uint32_t m_pool_slice;
    Task *m_current;
    uint32_t m_pid;
    Mutex m;
};

#endif
