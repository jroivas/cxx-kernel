#ifndef BOCHSFB_H
#define BOCHSFB_H

#include "fb.h"
#include "types.h"
#include "pci.hh"

class BochsFB : public FB
{
public:
    BochsFB(PCI *pci);
    ~BochsFB();

    FB::ModeConfig *query(FB::ModeConfig *prefer);
    virtual bool configure(ModeConfig *mode);
    virtual void clear();
    void blit();

    virtual bool isValid();
    virtual void initialize();

private:
    void setIORegister(uint16_t idx, uint16_t val);
    uint16_t getIORegister(uint16_t idx);
    void setMode(uint32_t width, uint32_t height, uint8_t bpp);
    bool validateMode(uint32_t width, uint32_t height, uint8_t bpp);
    void setBank(uint16_t bank);

    bool m_initialized;
    PCI *m_pci;
    PCI::HeaderGeneric *m_hg;
    uint32_t m_mmio_reg;
    uint32_t m_video;
    uint16_t m_bank;
};

#endif

