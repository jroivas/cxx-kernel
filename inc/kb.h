#ifndef KB_H
#define KB_H

#include "regs.h"

class KB
{
public:
    static KB *get();
    virtual ~KB() {};
    virtual bool hasKey();
    virtual const char *getKey();
    void startReading();
    void stopReading()
    {
        m_reading = false;
    }

protected:
    KB() : m_reading(false) { }
    static int handler(Regs *r);
    virtual void run(Regs *r) = 0;
    bool m_reading;
};

#endif
