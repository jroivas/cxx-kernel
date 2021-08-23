#include "bochs.h"
#include "types.h"
#include "port.h"
#include "mm.h"
#include "memcpy.h"
#include "port.h"

#include "string.hh"
#include "../platform.h"

#define PCI_BAR0 0x10
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
    m_hg = nullptr;
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
    if (!m_pci) {
        Platform::video()->printf("ERROR: PCI not initialized\n");
        return;
    }

    PCI::DeviceIterator *di = m_pci->startIter();
    if (!di)
        return;
    /* FIXME Bochs graphics can have other vendor / deviceid as well */
    m_hg = m_pci->findNextDeviceByVendor(di, 0x1234, 0x1111);
    if (!m_hg)
        return;

    m_video = m_pci->readValue32(m_hg, PCI_BAR0) & 0xfffffff0;
    // TODO Fix support for memory mapped control
#if 0
    m_mmio_reg = m_pci->readValue32(m_hg, PCI_BAR2) & 0xfffffff0;

    Paging p;
    bool ok;
    p.lock();

    ok = p.identityMap(m_mmio_reg);
    if (ok)
        ok = p.identityMap(m_mmio_reg + PAGE_SIZE);

    p.unlock();

    if (!ok) {
        Platform::video()->printf("Mapping failed\n");
        return;
    }
    m_initialized = true;
#endif
}

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

enum {
    VBE_DISPI_INDEX_ID       = 0,
    VBE_DISPI_INDEX_XRES     = 1,
    VBE_DISPI_INDEX_YRES     = 2,
    VBE_DISPI_INDEX_BPP      = 3,
    VBE_DISPI_INDEX_ENABLE   = 4,
    VBE_DISPI_INDEX_BANK     = 5,
    VBE_DISPI_INDEX_VWIDTH   = 6,
    VBE_DISPI_INDEX_VHEIGHT  = 7,
    VBE_DISPI_INDEX_X_OFFS   = 8,
    VBE_DISPI_INDEX_Y_OFFS   = 9,
};

void BochsFB::setIORegister(uint16_t idx, uint16_t val)
{
    Port::outw(VBE_DISPI_IOPORT_INDEX, idx);
    Port::outw(VBE_DISPI_IOPORT_DATA, val);
}

uint16_t BochsFB::getIORegister(uint16_t idx)
{
    Port::outw(VBE_DISPI_IOPORT_INDEX, idx);
    return Port::inw(VBE_DISPI_IOPORT_DATA);
}

void BochsFB::setMode(uint32_t width, uint32_t height, uint8_t bpp)
{
    Platform::video()->printf("=== Bochs set mode %ux%u %u\n",
        width, height, bpp);
    // FIXME Resolve proper PCI address
#if 0
    Bochs::MMIORegisters *regs = (Bochs::MMIORegisters *)m_mmio_reg;
    return;

    regs->regs.enable = 0;
    memory_barrier();

    regs->regs.xres = width;
    regs->regs.xres = height;
    regs->regs.virt_width = width;
    regs->regs.virt_height = height * 2;
    regs->regs.bpp = bpp;
    memory_barrier();

    // VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED
    regs->regs.enable = 1 | 0x40;
    memory_barrier();

    regs->regs.bank = 0;
#else
    setIORegister(VBE_DISPI_INDEX_ENABLE, 0);
    setIORegister(VBE_DISPI_INDEX_XRES, (uint16_t)width);
    setIORegister(VBE_DISPI_INDEX_YRES, (uint16_t)height);
    setIORegister(VBE_DISPI_INDEX_VWIDTH, width);
    setIORegister(VBE_DISPI_INDEX_VHEIGHT, height * 2);
    setIORegister(VBE_DISPI_INDEX_BPP, (uint16_t)bpp);

    setIORegister(VBE_DISPI_INDEX_ENABLE, 1 | 0x40);
    setIORegister(VBE_DISPI_INDEX_BANK, 0);
#endif
    m_bank = 0;
}

bool BochsFB::validateMode(uint32_t width, uint32_t height, uint8_t bpp)
{
    if (width != getIORegister(VBE_DISPI_INDEX_XRES))
        return false;
    if (height != getIORegister(VBE_DISPI_INDEX_YRES))
        return false;
    if (bpp != getIORegister(VBE_DISPI_INDEX_BPP))
        return false;

    return true;
}

FB::ModeConfig *BochsFB::query(FB::ModeConfig *prefer)
{
    MutexLocker lock(m);

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

    res->bytes_per_line = res->width * res->depth / 8;
    res->id = 1;

    Paging p;
    p.lock();

    // Map memory from phys addr m_video to new res->base
    uint32_t s = res->bytes_per_line * (res->height);
    ptr_val_t newbase;
    res->base = (unsigned char*)m_video;
    p.map(res->base, &newbase, 0x3);
    ptr_val_t tmpbase = 0;
    for (uint32_t i = PAGE_SIZE; i <= s; i += PAGE_SIZE) {
        p.map(res->base + i, &tmpbase, 0x3);
        if (newbase + i != tmpbase) {
            Platform::video()->printf("Discontinuation: %x\n", tmpbase);
            for (int j=0; j<0xffffff; j++) ;
        }
    }
    p.unlock();

    res->base = (unsigned char*)newbase;

    return res;
}

bool BochsFB::configure(ModeConfig *mode)
{
    if (mode == nullptr) return false;

    m->lock();
    initialize();

    setMode(mode->width, mode->height, mode->depth);
    if (!validateMode(mode->width, mode->height, mode->depth))
        return false;
    m->unlock();

    if (!FB::configure(mode))
        return false;

    m_configured = true;
    clear();
    return true;
}

void BochsFB::setBank(uint16_t bank)
{
    if (bank == m_bank)
        return;

    setIORegister(VBE_DISPI_INDEX_BANK, bank);
    m_bank = bank;
}

void BochsFB::blit()
{
    if (m_direct) return;
    if (!m_current) return;

    MutexLocker lock(m);
#if 1
    memcpy_opt(m_current->base, m_buffer, m_size);
#else
    Mem::copy(m_current->base, m_buffer, m_size);
#endif
}

void BochsFB::clear()
{
    if (!m_current) return;

    MutexLocker lock(m);
    Mem::set(m_current->base, 0, m_size);
}
