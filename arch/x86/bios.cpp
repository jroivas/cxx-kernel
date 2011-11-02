#include "bios.h"
#include "mm.h"
#include "paging.h"
#include "port.h"
#include "string.h"
#include "../platform.h"

#if 0
#ifdef __cplusplus
extern "C" {
#endif
//#include "x86emu/x86emu.h"
#include "x86emu.h"
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

#if 1
#if 0
#ifdef __cplusplus
extern "C" {
#endif
#endif
static uint8_t bios_x86emu_pio_inb(struct x86emu *, uint16_t port) {
        return Port::in(port);
}

static uint16_t bios_x86emu_pio_inw(struct x86emu *, uint16_t port) {
        return Port::inw(port);
}

static uint32_t bios_x86emu_pio_inl(struct x86emu *, uint16_t port) {
        return Port::inl(port);
}

static void bios_x86emu_pio_outb(struct x86emu *, uint16_t port, uint8_t data) {
        Port::out(port, data);
}

static void bios_x86emu_pio_outw(struct x86emu *, uint16_t port, uint16_t data) {
        Port::outw(port, data);
}

static void bios_x86emu_pio_outl(struct x86emu *, uint16_t port, uint32_t data) {
        Port::outl(port, data);
}

static uint8_t bios_x86emu_mem_rdb(struct x86emu *, uint32_t addr) {
        return *(uint8_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping()));
}

static void bios_x86emu_mem_wrb(struct x86emu *, uint32_t addr, uint8_t val) {
        *(uint8_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping())) = val;
}

static uint16_t bios_x86emu_mem_rdw(struct x86emu *, uint32_t addr) {
        return *(uint16_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping()));
}

static void bios_x86emu_mem_wrw(struct x86emu *, uint32_t addr, uint16_t val) {
        *(uint16_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping())) = val;
}

static uint32_t bios_x86emu_mem_rdl(struct x86emu *, uint32_t addr) {
        return *(uint32_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping()));
}

static void bios_x86emu_mem_wrl(struct x86emu *, uint32_t addr, uint32_t val) {
        *(uint32_t *)((ptr_t)(addr + (ptr_val_t)BIOS::get()->memMapping())) = val;
}


/*
static X86EMU_pioFuncs bios_x86emu_pio_funcs = {
        .inb  = bios_x86emu_pio_inb,
        .outb = bios_x86emu_pio_outb,
        .inw  = bios_x86emu_pio_inw,
        .outw = bios_x86emu_pio_outw,
        .inl  = bios_x86emu_pio_inl,
        .outl = bios_x86emu_pio_outl,
};
*/
#if 0
#ifdef __cplusplus
}
#endif
#endif
#endif

static BIOS *__static_bios = NULL;

BIOS *BIOS::get()
{
	if (__static_bios==NULL) {
		__static_bios = new BIOS();
	}
	return __static_bios;
}

BIOS::BIOS()
{
	Platform::video()->printf("PRE A\n");
	mem_mapping = MM::instance()->alloc(0x100000, MM::AllocFast);
	//mem_mapping = MM::instance()->alloc(0x100000);
	//mem_mapping = MM::instance()->alloc(5001, MM::AllocFast);
	Platform::video()->printf("POST A: %x\n",mem_mapping);
	if (mem_mapping==NULL) return;
	//return;

#if 0
	Platform::video()->printf("PRE B\n");
	Paging p;
	p.lock();
	bios_pages = p.alloc(BIOS_MEM_SIZE/PAGE_SIZE+1, 0, Paging::PagingAllocDontMap);
	p.unlock();
#endif

	Platform::video()->printf("PRE D\n");
	mapMem((ptr_t)BIOS_BDA_BASE, BIOS_BDA_BASE, BIOS_BDA_SIZE);
	//mapMem((ptr_t)BIOS_MEM_BASE, (phys_ptr_t)bios_pages, BIOS_MEM_SIZE);
	mapMem((ptr_t)BIOS_MEM_BASE, (phys_ptr_t)bios_pages, BIOS_MEM_SIZE);
	mapMem((ptr_t)BIOS_EBDA_BASE, BIOS_EBDA_BASE, BIOS_EBDA_SIZE);
	Platform::video()->printf("PRE D3\n");

	//X86EMU_setupPioFuncs(&x86emu_pio_funcs);
	setupX86EMU();
	Platform::video()->printf("PRE E\n");
}

void BIOS::setupX86EMU()
{
#if 1
#if 0
#if 0
	(void)bios_x86emu_pio_funcs;
#else
	X86EMU_setupPioFuncs(&bios_x86emu_pio_funcs);
#endif
#else
	/* Filling up the struct */
	x86emu mach;

	//x86emu_init_default(&mach);

	mach.emu_inb = bios_x86emu_pio_inb;
	mach.emu_inw = bios_x86emu_pio_inw;
	mach.emu_inl = bios_x86emu_pio_inl;
	mach.emu_outb = bios_x86emu_pio_outb;
	mach.emu_outw = bios_x86emu_pio_outw;
	mach.emu_outl = bios_x86emu_pio_outl;

	mach.emu_rdb = bios_x86emu_mem_rdb;
	mach.emu_rdw = bios_x86emu_mem_rdw;
	mach.emu_rdl = bios_x86emu_mem_rdl;
	mach.emu_wrb = bios_x86emu_mem_wrb;
	mach.emu_wrw = bios_x86emu_mem_wrw;
	mach.emu_wrl = bios_x86emu_mem_wrl;

	mach.mem_base = (char*)mem_mapping;
	mach.mem_size = 0x100000;
	Mem::set(&mach.x86, 0, sizeof(struct x86emu_regs));
	mach.x86.register_flags = (1<<9) | (1<<1);

	mach.x86.register_a.I16_reg.x_reg = 0x4F03;

	//x86emu_exec_intr(&mach, 0x10);
#endif
#endif
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
