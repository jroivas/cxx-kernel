#include "waitlist.hh"
#include "platform.h"
#include "task.h"

void Waitlist::enqueue()
{
    Task *current = Platform::processManager()->currentTask();

    current->wait.next = nullptr;
    if (!tail) {
        tail = head = &current->wait;
    } else {
        tail = tail->next = &current->wait;
    }

    // FIXME
    /* Scheduling stop now */
    current->setStatus(Task::STATUS_BLOCKED);
}

void Waitlist::dequeue()
{
    if (head == nullptr) {
        Platform::video()->printf("Dequeue empty!\n");
        return;
    }
        
    Task *next = head->task;
    if (head == tail) {
        head = tail = nullptr;
    } else {
        head = head->next;
    }

    if (!next) {
        Platform::video()->printf("Next dequeue failed\n");
        return;
    }

    /* Enable scheduling now */
    if (next->status() != Task::STATUS_BLOCKED) {
        Platform::video()->printf("Dequeue invalid status: %u\n", next->status());
    }
    next->setStatus(Task::STATUS_WAITING);
}

void Waitlist::abort()
{
}

bool Waitlist::empty()
{
    return !head;
}
