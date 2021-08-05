#ifndef KB_X86_H
#define KB_X86_H

#include "kb.h"

class KBX86 : public KB
{
public:
    KBX86();
    virtual ~KBX86() {}
    bool hasKey();
    const char *getKey();

protected:
    void run(Regs *r);
};
#endif
