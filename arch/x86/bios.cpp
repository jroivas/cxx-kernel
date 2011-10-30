#include "bios.h"
#include "mm.h"
#include "paging.h"
#include "port.h"
#include "../platform.h"

#if 0
#ifdef __cplusplus
extern "C" {
#endif
#include "x86emu/x86emu.h"
#ifdef __cplusplus
}
#endif
#else
#include "x86emu.h"
#endif

#define BIOS_BDA_BASE           0
#define BIOS_BDA_SIZE           0x1000
#define BIOS_EBDA_BASE          0x90000
#define BIOS_EBDA_SIZE          0x70000
#define BIOS_MEM_BASE           0x1000
#define BIOS_MEM_SIZE           0x8F000
#define BIOS_STACK_SIZE         0x1000

#if 0
#ifdef __cplusplus
extern "C" {
#endif
#endif
static uint8_t x86emu_pio_inb(uint16_t port) {
        return Port::in(port);
}

static uint16_t x86emu_pio_inw(uint16_t port) {
        return Port::inw(port);
}

static uint32_t x86emu_pio_inl(uint16_t port) {
        return Port::inl(port);
}

static void x86emu_pio_outb(uint16_t port, uint8_t data) {
        Port::out(port, data);
}

static void x86emu_pio_outw(uint16_t port, uint16_t data) {
        Port::outw(port, data);
}

static void x86emu_pio_outl(uint16_t port, uint32_t data) {
        Port::outl(port, data);
}

static X86EMU_pioFuncs x86emu_pio_funcs = {
        .inb  = x86emu_pio_inb,
        .outb = x86emu_pio_outb,
        .inw  = x86emu_pio_inw,
        .outw = x86emu_pio_outw,
        .inl  = x86emu_pio_inl,
        .outl = x86emu_pio_outl,
};
#if 0
#ifdef __cplusplus
}
#endif
#endif

BIOS::BIOS()
{
	Platform::video()->printf("PRE A\n");
	mem_mapping = MM::instance()->alloc(0x100000, MM::AllocFast);
	if (mem_mapping==NULL) return;

	Platform::video()->printf("PRE B\n");
	Paging p;
	p.lock();
	bios_pages = p.alloc(BIOS_MEM_SIZE/PAGE_SIZE+1, 0, Paging::PagingAllocDontMap);
	p.unlock();
	return;

	Platform::video()->printf("PRE D\n");
	mapMem((ptr_t)BIOS_BDA_BASE, BIOS_BDA_BASE, BIOS_BDA_SIZE);
	mapMem((ptr_t)BIOS_MEM_BASE, (phys_ptr_t)bios_pages, BIOS_MEM_SIZE);
	mapMem((ptr_t)BIOS_EBDA_BASE, BIOS_EBDA_BASE, BIOS_EBDA_SIZE);
	Platform::video()->printf("PRE D3\n");

	//X86EMU_setupPioFuncs(&x86emu_pio_funcs);
	setupX86EMU();
	Platform::video()->printf("PRE E\n");
}

void BIOS::setupX86EMU()
{
	(void)x86emu_pio_funcs;
	X86EMU_setupPioFuncs(&x86emu_pio_funcs);
}

void BIOS::mapMem(ptr_t addr, phys_ptr_t phys, size_t size)
{
	Paging p;
	p.lock();
	for (size_t i=0; i<size; i+=PAGE_SIZE) {
		p.map((ptr_t)((ptr_val_t)mem_mapping + addr + i), (ptr_t)((ptr_val_t)phys + i), 0x3); //FIXME
	}
	p.unlock();
}
