#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "mutex.hh"

//#define TASK_MAGIC 0x42
#define MAX_PRIORITY 20
#define MAX_NICE     20
#define TASK_NAME_LEN 42

class Task
{
public:
    enum CloneFlags {
        CLONE_NORMAL        = 0,
        CLONE_SHARE_ADDRESS = (1<<0),
        CLONE_SHARE_STACK   = (1<<1),
        CLONE_SHARE_FS      = (1<<2),
        CLONE_SHARE_FILES   = (1<<3),
        CLONE_SHARE_SIGNAL  = (1<<4),
        CLONE_SHARE_PARENT  = (1<<5)
    };

    Task();
    virtual void init(ptr_val_t addr, ptr_val_t stack, uint32_t flags) = 0;

    virtual void switchTo(volatile ptr_val_t *lock, ptr_t killer) = 0;
    virtual int save() = 0;
    virtual int restore(volatile ptr_val_t *lock) = 0;

    void setName(const char *n)
    {
        for (uint32_t i=0; i < TASK_NAME_LEN - 1; i++) {
            m_name[i] = n[i];
        }
        m_name[TASK_NAME_LEN - 1] = 0;
    }
    const char *name()
    {
        return m_name;
    }

    virtual Task *clone(CloneFlags flags = CLONE_NORMAL) = 0;
    virtual Task *create(ptr_val_t addr, ptr_val_t stack, uint32_t flags) = 0;

    virtual void setEntry(ptr_val_t addr) = 0;

    inline void setSize(uint32_t s)
    {
        m_size = s;
    }
    inline uint32_t size() const
    {
        return m_size;
    }

    void setSlice(uint32_t s);
    inline uint32_t slice() const
    {
        return m_slice;
    }
    inline void decSlice()
    {
        if (m_slice>0) {
            m_slice--;
        }
    }

    inline uint32_t priority() const
    {
        return m_priority;
    }
    inline void setPriority(uint32_t p)
    {
        if (p > MAX_PRIORITY) {
            p = MAX_PRIORITY;
        }
        m_priority = p;
    }

    inline uint32_t nice() const
    {
        return m_nice;
    }
    inline void setNice(uint32_t p)
    {
        if (p > MAX_NICE) {
            p = MAX_NICE;
        }
        m_nice = p;
    }

    inline void setPid(uint32_t p)
    {
        m_pid = p;
    };
    inline uint32_t pid() const
    {
        return m_pid;
    }

    inline volatile ptr_val_t *getLock()
    {
        return &m_lock;
    }
    void lock();
    void unlock();

protected:
    ptr_val_t m_stack;
    ptr_val_t m_stack_syscall;
    ptr_val_t m_entry;
    uint32_t m_priority;
    uint32_t m_nice;

    uint32_t m_size;
    uint32_t m_slice;
    uint32_t m_pid;
    volatile ptr_val_t m_lock;
    char m_name[TASK_NAME_LEN];
    Mutex m_m;
    bool m_userSpace;
};

inline void Task::lock()
{
    m_m.lock();
}
inline void Task::unlock()
{
    m_m.unlock();
}
inline void Task::setSlice(uint32_t s)
{
    if (m_size != 0 && s > m_size) {
        m_slice = m_size;
    } else {
        m_slice = s;
    }
}

#endif
