#ifndef _PCI_X86_H
#define _PCI_X86_H

#include "pci.hh"

class PCIX86 : public PCI
{
public:
    PCIX86();
    virtual ~PCIX86();

protected:
    virtual void systemPut(uint32_t val);
    virtual void systemPutData(uint8_t val);
    virtual uint32_t systemGet();
};

#endif
