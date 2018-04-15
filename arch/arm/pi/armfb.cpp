#include "mm.h"
#include "string.hh"
#include "../platform.h"
#include "armfb.h"

typedef struct _PL110MMIO
{
        uint32_t          volatile tim0;          //0
        uint32_t          volatile tim1;          //4
        uint32_t          volatile tim2;          //8
        uint32_t          volatile d;             //c
        uint32_t          volatile upbase;        //10
        uint32_t          volatile f;             //14
        uint32_t          volatile g;             //18
        uint32_t          volatile control;       //1c
} PL110MMIO;

#define PL110_CR_EN             0x001
#define PL110_CR_PWR            0x800
//#define PL110_IOBASE            0xc0000000
#define PL110_IOBASE            0x10120000
#define PL110_PALBASE           (PL110_IOBASE + 0x200)
#define PL110_UPBASE            0x2000000
//#define PL110_UPBASE            (1 * 1024 * 1024)

//#define PLIO(X) ((PL110MMIO*)X)
#define PLIO(X) X

ARMFB::ARMFB() : FB()
{
	//base = ((uint16_t*)PL110_UPBASE);
	//plio = (PL110MMIO*)PL110_IOBASE;
	//plio = (void*)PL110_IOBASE;
	PL110MMIO* plio = (PL110MMIO*)PL110_IOBASE;

#if 0
        /* 640x480 pixels */
        PLIO(plio)->tim0 = 0x3f1f3f9c;
        PLIO(plio)->tim1 = 0x080b61df;
        PLIO(plio)->upbase = PL110_UPBASE;
        /* 16-bit color */
        PLIO(plio)->control = 0x1829;
#else
	//800x600
	*(volatile unsigned int *)(0x1000001C) = 0x2CAC;
        PLIO(plio)->tim0 = 0x1313A4C4;
        PLIO(plio)->tim1 = 0x0505F657;
        PLIO(plio)->tim2 = 0x071F1800;
        PLIO(plio)->upbase = PL110_UPBASE;
        PLIO(plio)->g = 0x82b;
#endif
}

ARMFB::~ARMFB()
{
}

FB::ModeConfig *ARMFB::query(FB::ModeConfig *prefer)
{
	(void)prefer;
	FB::ModeConfig *conf = (FB::ModeConfig*)MM::instance()->alloc(sizeof(FB::ModeConfig));
	conf->width = 800;
	conf->height = 600;
	conf->depth = 32;
	conf->bytes_per_line = conf->width * conf->depth/8;
	conf->base = (uint8_t*)PL110_UPBASE;
	return conf;
}

bool ARMFB::configure(ModeConfig *mode)
{
	if (mode==nullptr) return false;

	m_current = mode;
	setDirect();
	bool r = FB::configure(mode);
	setDirect();
#if 0
	if (m_buffer!=nullptr) {
		uint32_t *s = (uint32_t*)m_buffer;
		uint32_t volatile *dest;
		dest = (uint32_t *)PL110_UPBASE;
		for (int32_t i=0; i<(m_current->width*m_current->height); ++i) {
			*s++=0xff0000;
			//*s2++=0;
			//dest[i]=0xff00;
		}
	}
#endif
	return r;
}

void ARMFB::clear()
{
}

void ARMFB::blit()
{
	if (m_direct) return;

	//m.lock();
	uint32_t *s = (uint32_t*)m_buffer;
	uint32_t volatile *dest;
	dest = (uint32_t *)PL110_UPBASE;
	for (int32_t i=0; i<(m_current->width*m_current->height); ++i) {
		dest[i]=*s++;
		//dest[i]=0xff;
	}
	//m.unlock();
}
