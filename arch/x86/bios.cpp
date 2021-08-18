#include "bios.h"
#include "mm.h"
#include "paging.h"
#include "port.h"
#include "string.hh"
#include "../platform.h"

#include "x86emu.h"
#include "x86emu_regs.h"

#define BIOS_MEM_BASE           0x1000
#define BIOS_MEM_SIZE           0x8F000
#define BIOS_STACK_SIZE         0x1000

static uint8_t bios_x86emu_pio_inb(struct x86emu *, uint16_t port)
{
    return Port::in(port);
}

static uint16_t bios_x86emu_pio_inw(struct x86emu *, uint16_t port)
{
    return Port::inw(port);
}

static uint32_t bios_x86emu_pio_inl(struct x86emu *, uint16_t port)
{
    return Port::inl(port);
}

static void bios_x86emu_pio_outb(struct x86emu *, uint16_t port, uint8_t data)
{
    Port::out(port, data);
}

static void bios_x86emu_pio_outw(struct x86emu *, uint16_t port, uint16_t data)
{
    Port::outw(port, data);
}

static void bios_x86emu_pio_outl(struct x86emu *, uint16_t port, uint32_t data)
{
    Port::outl(port, data);
}

static uint8_t bios_x86emu_mem_rdb(struct x86emu *emu, uint32_t addr)
{
    if (emu == nullptr) return 0;
    if (addr > emu->mem_size - 1) {
        x86emu_halt_sys(emu);
    }
    return emu->mem_base[addr];
}

static void bios_x86emu_mem_wrb(struct x86emu *emu, uint32_t addr, uint8_t val)
{
    if (emu == nullptr) return;
    if (addr > emu->mem_size - 1) {
        x86emu_halt_sys(emu);
    }
    emu->mem_base[addr] = val;
}

static uint16_t bios_x86emu_mem_rdw(struct x86emu *emu, uint32_t addr)
{
    if (emu == nullptr) return 0;
    if (addr > emu->mem_size - 2) {
        x86emu_halt_sys(emu);
    }
    return *(uint16_t *)(emu->mem_base + addr);
}

static void bios_x86emu_mem_wrw(struct x86emu *emu, uint32_t addr, uint16_t val)
{
    if (emu == nullptr) return;
    if (addr > emu->mem_size - 2) {
        x86emu_halt_sys(emu);
    }
    *((uint16_t *)(emu->mem_base + addr)) = val;
}

static uint32_t bios_x86emu_mem_rdl(struct x86emu *emu, uint32_t addr)
{
    if (emu==nullptr) return 0;
    if (addr > emu->mem_size - 4) {
        x86emu_halt_sys(emu);
    }
    return *(uint32_t *)(emu->mem_base + addr);
}

static void bios_x86emu_mem_wrl(struct x86emu *emu, uint32_t addr, uint32_t val)
{
    if (emu==nullptr) return;
    *((uint32_t *)(emu->mem_base + addr)) = val;
}


static BIOS *__static_bios = nullptr;
static Mutex __bios_mutex;

BIOS *BIOS::get()
{
    if (__static_bios==nullptr) {
        __static_bios = new BIOS();
    }
    return __static_bios;
}

BIOS::BIOS()
{
    free_base = BIOS_MEM_BASE;
    (void)bios_pages;

    bios_stack = (void*)alloc(BIOS_STACK_SIZE);
    bios_halt = (void*)alloc(1);
    if (bios_stack != nullptr && bios_halt != nullptr) {
        *(uint8_t*)bios_halt = 0xF4;
    }
}

extern "C" unsigned int get_esp();
void BIOS::setupX86EMU(void *ptr)
{
    /* Filling up the struct */
    x86emu *mach = (x86emu*)ptr;

    //x86emu_init_default(&mach);

    mach->emu_inb = bios_x86emu_pio_inb;
    mach->emu_inw = bios_x86emu_pio_inw;
    mach->emu_inl = bios_x86emu_pio_inl;
    mach->emu_outb = bios_x86emu_pio_outb;
    mach->emu_outw = bios_x86emu_pio_outw;
    mach->emu_outl = bios_x86emu_pio_outl;

    mach->emu_rdb = bios_x86emu_mem_rdb;
    mach->emu_rdw = bios_x86emu_mem_rdw;
    mach->emu_rdl = bios_x86emu_mem_rdl;
    mach->emu_wrb = bios_x86emu_mem_wrb;
    mach->emu_wrw = bios_x86emu_mem_wrw;
    mach->emu_wrl = bios_x86emu_mem_wrl;

    //mach->mem_base = (char*)mem_mapping;
    mach->mem_base = (char*)0;
    //mach->mem_size = 0x100000;
    mach->mem_size = 0xFFFFF;
    Mem::set(&mach->x86, 0, sizeof(struct x86emu_regs));
    //mach->x86.R_EFLG = (1<<9) | (1<<1);
    mach->x86.R_EFLG = F_IF;

    mach->x86.R_ESP = ((uint32_t)bios_stack) + BIOS_STACK_SIZE;
    *(uint8_t*)bios_halt = 0xF4;
    mach->x86.R_EIP = (uint32_t)bios_halt;
}

void *BIOS::alloc(uint32_t size)
{
    if (free_base < BIOS_MEM_BASE + BIOS_MEM_SIZE) {
        __bios_mutex.lock();
#ifdef ALIGN_BIOS
        while ((free_base % PAGE_SIZE) != 0) {
            free_base++;
        }
#endif
        void *tmp = (void*)free_base;
        free_base += size;

        __bios_mutex.unlock();

        return tmp;
    }
    return nullptr;
}

bool BIOS::runInt(uint32_t interrupt, Regs *regs)
{
    if (interrupt == 0) return false;
    if (bios_stack == nullptr || bios_halt == nullptr) {
        return false;
    }

    __bios_mutex.lock();

    x86emu mach;
    Mem::set(&mach, 0, sizeof(struct x86emu));
    setupX86EMU(&mach);

    if (regs != nullptr) {
        mach.x86.R_EAX = regs->eax;
        mach.x86.R_EBX = regs->ebx;
        mach.x86.R_ECX = regs->ecx;
        mach.x86.R_EDX = regs->edx;

        mach.x86.R_ES = (uint16_t)regs->es;
        mach.x86.R_FS = (uint16_t)regs->fs;
        mach.x86.R_DS = (uint16_t)regs->ds;
        mach.x86.R_GS = (uint16_t)regs->gs;

        mach.x86.R_EDI = regs->edi;
        mach.x86.R_ESI = regs->esi;
        mach.x86.R_EBP = regs->ebp;
    }

    x86emu_exec_intr(&mach, (uint8_t)interrupt);

    if (regs != nullptr) {
        regs->eax = mach.x86.R_EAX;
        regs->ebx = mach.x86.R_EBX;
        regs->ecx = mach.x86.R_ECX;
        regs->edx = mach.x86.R_EDX;

        regs->es = mach.x86.R_ES;
        regs->fs = mach.x86.R_FS;
        regs->ds = mach.x86.R_DS;
        regs->gs = mach.x86.R_GS;

        regs->edi = mach.x86.R_EDI;
        regs->esi = mach.x86.R_ESI;
        regs->ebp = mach.x86.R_EBP;
    }

    __bios_mutex.unlock();

    return true;
}
