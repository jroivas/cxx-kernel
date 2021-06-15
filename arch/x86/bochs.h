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

private:
    void initialize();
    void setMode(uint32_t width, uint32_t height, uint8_t bpp);

    bool m_initialized;
    PCI *m_pci;
    PCI::HeaderGeneric *m_hg;
    uint32_t m_mmio_reg;
};

#endif

