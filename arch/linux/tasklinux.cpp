#include "tasklinux.h"
#include "platform.h"

TaskLinux::TaskLinux() : Task()
{
    created = false;
}

void TaskLinux::init(ptr_val_t addr, ptr_val_t stack, uint32_t flags)
{
    (void)stack;
    (void)flags;
    entry = addr;
}

void TaskLinux::setEntry(ptr_val_t addr)
{
    entry = addr;
}

void TaskLinux::switchTo(volatile ptr_val_t *lock, ptr_t killer)
{
    (void)lock;
    (void)killer;
    if (created) return;
    if (pthread_create(&thread, NULL, (void* (*)(void*))entry, this) == 0) {
        created = true;
    }
}

int TaskLinux::save()
{
    return 0;
}

int TaskLinux::restore(volatile ptr_val_t *lock)
{
    (void)lock;
    return 0;
}

Task *TaskLinux::clone(TaskLinux::CloneFlags flags)
{
    (void)flags;
    return nullptr;
}

Task *TaskLinux::create(ptr_val_t addr, ptr_val_t stack, uint32_t flags)
{
    TaskLinux *t = new TaskLinux();
    if (t == nullptr) return nullptr;

    t->init(addr, stack, flags);
    return t;
}
