#pragma once

class Task;
class WaitItem
{
public:
    WaitItem() : task(nullptr), next(nullptr) {}

    Task *task;
    WaitItem *next;
};

class Waitlist
{
    friend class WaitItem;
public:
    Waitlist() : head(nullptr), tail(nullptr) {}
    void enqueue();
    void dequeue();
    void abort();
    bool empty();

private:
    WaitItem *head;
    WaitItem *tail;
};
