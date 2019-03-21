#pragma once

#include "types.h"
#include "task.h"
#include <pthread.h>

class TaskLinux : public Task
{
public:
    TaskLinux();

    void init(ptr_val_t addr, ptr_val_t stack, uint32_t flags);
    void switchTo(volatile ptr_val_t *lock, ptr_t killer);
    int save();
    int restore(volatile ptr_val_t *lock);
    Task *clone(CloneFlags flags = CLONE_NORMAL);
    Task *create(ptr_val_t addr, ptr_val_t stack, uint32_t flags);
    void setEntry(ptr_val_t addr);

protected:
    bool created;
    ptr_val_t entry;
    pthread_t thread;
};
