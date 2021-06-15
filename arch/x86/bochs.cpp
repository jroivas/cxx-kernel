#include "bochs.h"
#include "types.h"
#include "port.h"
#include "mm.h"
#include "memcpy.h"

#include "string.hh"
#include "../platform.h"

#define PCI_BAR2 0x18

namespace Bochs {

    typedef struct DispInterface {
        uint16_t index_id;
        uint16_t xres;
        uint16_t yres;
        uint16_t bpp;
        uint16_t enable;
        uint16_t bank;
        uint16_t virt_width;
        uint16_t virt_height;
        uint16_t x_offset;
        uint16_t y_offset;
    } __attribute__((packed)) DispInterface;

    typedef struct MMIORegisters {
        uint8_t edid[0x400];
        uint16_t ioports[0x10];
        uint8_t reserved[0xE0];
        DispInterface regs;
    } __attribute__((packed)) MMIORegisters;
}


BochsFB::BochsFB(PCI *pci) : FB(), m_initialized(false), m_pci(pci)
{
    PCI::DeviceIterator *di = m_pci->startIter();
    m_hg = m_pci->findNextDeviceByVendor(di, 0x1234, 0x1111);
}

BochsFB::~BochsFB()
{
}

bool BochsFB::isValid()
{
    return m_hg != nullptr;
}

void BochsFB::initialize()
{
    if (m_initialized)
        return;
    m_mmio_reg = m_pci->readValue32(m_hg, PCI_BAR2);
    m_initialized = true;
    //setMode(1024, 768);
}

void BochsFB::setMode(uint32_t width, uint32_t height, uint8_t bpp)
{
    Bochs::MMIORegisters *regs = (Bochs::MMIORegisters *)m_mmio_reg;
    Platform::video()->printf("=== Bochs set mode %ux%u\n",
        width, height);
    return;

    regs->regs.enable = 0;
    memory_barrier();

    regs->regs.xres = width;
    regs->regs.xres = height;
    regs->regs.virt_width = width;
    regs->regs.virt_height = height * 2;
    regs->regs.bpp = bpp;
    memory_barrier();

    //VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED
    regs->regs.enable = 1 | 0x40;
    memory_barrier();

    regs->regs.bank = 0;
}

FB::ModeConfig *BochsFB::query(FB::ModeConfig *prefer)
{
    m.lock();

    FB::ModeConfig *res = (FB::ModeConfig *)MM::instance()->alloc(sizeof(FB::ModeConfig));

    if (prefer)  {
        res->width = prefer->width;
        res->height = prefer->height;
        switch (prefer->depth) {
        case 8:
        case 16:
        case 24:
        case 32:
            res->depth = prefer->depth;
            break;
        default:
            res->depth = 32;
            break;
        }
    } else {
        res->width = 1024;
        res->height = 768;
        res->depth = 32;
    }

    res->base = 0;
    res->bytes_per_line = 1024 * res->depth / 4;
    res->id = 1;

    return res;
}

bool BochsFB::configure(ModeConfig *mode)
{
    setMode(mode->width, mode->height, mode->depth);
    return true;
}

void BochsFB::blit()
{
    // TODO
}

void BochsFB::clear()
{
    // TODO
}
