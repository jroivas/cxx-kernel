#ifndef _ATA_X86_H
#define _ATA_X86_H

#include "ata.hh"

class ATAX86 : public ATA
{
public:
    ATAX86();
    ~ATAX86();

protected:
    virtual void systemPortOut(uint32_t port, uint8_t val);
    virtual void systemPortOut16(uint32_t port, uint16_t val);
    virtual void systemPortOut32(uint32_t port, uint32_t val);
    virtual uint8_t systemPortIn(uint32_t port);
    virtual uint16_t systemPortIn16(uint32_t port);
    virtual uint32_t systemPortIn32(uint32_t port);
};

#endif
