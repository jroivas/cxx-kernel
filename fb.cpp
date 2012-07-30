#include "fb.h"
#include "string.h"
#include "types.h"
#include "mm.h"
#include "arch/platform.h"
#include "memcopy.h"

static volatile ptr_val_t __fb_mutex = 0;

FB::FB()
{
	m_current = NULL;
	m_buffer = NULL;
	m_backbuffer = NULL;
	m_double_buffer = true;
	m_direct = false;
	m_configured = false;
	m.assign(&__fb_mutex);
}

FB::~FB()
{
	freeBuffers();
}

void FB::setSingleBuffer()
{
	m.lock();

	m_double_buffer = false;
	if (m_buffer!=NULL) {
		MM::instance()->free(m_buffer);
	}
	m_buffer = NULL;

	m.unlock();
}

void FB::setDirect()
{
	m.lock();

	m_direct = true;
	if (m_backbuffer!=NULL) {
		MM::instance()->free(m_backbuffer);
	}
	if (m_buffer!=NULL) {
		MM::instance()->free(m_buffer);
	}
	m_backbuffer = m_current->base;
	m_buffer = m_current->base;

	m.unlock();
}

void FB::allocBuffers()
{
	if (m_direct) return;
	m.lock();

	m_size = m_current->bytes_per_line*(m_current->height);
	m_backbuffer = (unsigned char*)MM::instance()->alloc(m_size, MM::AllocClear);
	//m_backbuffer = (unsigned char*)MM::instance()->alloc(m_size);
	if (m_backbuffer!=NULL) {
		m_backbuffer[0] = 0xfe;
		m_backbuffer[m_size/2] = 0xed;
		m_backbuffer[m_size-1] = 0xdc;
		if (m_backbuffer==NULL || m_backbuffer[0] != 0xfe || m_backbuffer[m_size/2] != 0xed || m_backbuffer[m_size-1] != 0xdc) {
			m_direct = true;
			MM::instance()->free(m_backbuffer);
			m_backbuffer = m_current->base;
			//Platform::video()->printf("direct\n");
		} else {
			m_backbuffer[0] = 0;
			m_backbuffer[m_size/2] = 0;
			m_backbuffer[m_size-1] = 0;
			m_buffer = (unsigned char*)MM::instance()->alloc(m_size, MM::AllocClear);
			m_buffer[0] = 0xef;
			m_buffer[m_size/2] = 0xde;
			m_buffer[m_size-1] = 0xec;
			if (m_buffer==NULL || m_buffer[0] != 0xef || m_buffer[m_size/2] != 0xde || m_buffer[m_size-1] != 0xec) {
				m_double_buffer = false;
				MM::instance()->free(m_buffer);
				m_buffer = NULL;
				//m_buffer = m_backbuffer;
				//Platform::video()->printf("singlebuffer\n");
			} else {
				m_buffer[0] = 0;
				m_buffer[m_size/2] = 0;
				m_buffer[m_size-1] = 0;
			}
		}
	}
#if 0
	for (uint32_t i=0; i<m_size; i++) {
		if (buffer!=NULL) buffer[i] = 0;
		if (backbuffer!=NULL) backbuffer[i] = 0;
	}
#endif
	//Platform::video()->printf("Allocbuffers: %x %x %x sizes: %d\n",m_current->base,m_buffer,m_backbuffer,m_current->bytes_per_line*(m_current->height));
	m.unlock();
}

void FB::freeBuffers()
{
	m.lock();
	if (m_buffer!=NULL) {
		MM::instance()->free(m_buffer);
		m_buffer = NULL;
	}
	if (m_backbuffer!=NULL) {
		MM::instance()->free(m_backbuffer);
		m_backbuffer = NULL;
	}
	m.unlock();
}

bool FB::configure(ModeConfig *mode)
{
	if (mode==NULL) return false;

	m_current = mode;

	allocBuffers();

	m_configured = true;
	return true;
}

unsigned char *FB::data()
{
	if (m_buffer == NULL) return NULL;
	return m_buffer;
}

void FB::swap()
{
	if (m_buffer == NULL) return;
	if (m_backbuffer == NULL) return;
	if (m_direct) return;
	if (!m_double_buffer) {
		m_buffer = m_backbuffer;
		return;
	}

	m.lock();
	if (m_buffer!=NULL && !m_direct) {
		memcpy_opt(m_buffer, m_backbuffer, m_size);
		//Mem::copy(m_buffer, m_backbuffer, m_size);
	}
	m.unlock();
}

#if 0
void printD(uint32_t d)
{
	char aa[5];
	while (d>0) {
		uint32_t i = d%10;
		d/=10;
		aa[0] = '0'+i;
		aa[1] = 0;
		Video::get()->printf(aa);
	}
	Video::get()->printf("\n");
}
#endif

void FB::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (m_backbuffer==NULL) return;
	if (m_current==NULL) return;

/*
	x %= m_current->width;
	y %= m_current->height;
*/
	if (x>m_current->width) x = m_current->width;
	if (y>m_current->height) y = m_current->height;

	switch (m_current->depth) {
		case 16:
			{
			unsigned short *pos = (unsigned short*)(m_backbuffer+(y*m_current->bytes_per_line + x*2));
			unsigned short color = 0;
			color += (r & 0x7C) << 8;
			color += (g & 0x7E) << 2;
			color += (b & 0x7C) >> 3;
			*pos = color;
			}
			break;
		/*case 24:
			{
			unsigned char *pos = (unsigned char*)(backbuffer+(y*current->bytes_per_line + x*4));
			*pos++ = r;
			*pos++ = g;
			*pos++ = b;
			}
			break;*/
		case 24:
			{
			unsigned char *pos = (unsigned char*)(m_backbuffer+(y*m_current->bytes_per_line + x*3));
			*pos++ = r;
			*pos++ = g;
			*pos = b;
			}
			break;
		case 32:
			{
			uint32_t *pos = (uint32_t*)(m_backbuffer+(y*m_current->bytes_per_line + x*4));
			uint32_t color = (a<<24)+(r<<16)+(g<<8)+b;
			*pos = color;
			}
			break;
		default:
			//Platform::video()->printf("Unsupported depth: %d\n",m_current->depth);
			// Unsupported
			break;
	}
}

void FB::putPixel(int x, int y, unsigned int color)
{
	if (m_backbuffer!=NULL && m_current!=NULL && m_current->depth==32) {
		x %= m_current->width;
		y %= m_current->height;
		uint32_t *pos = (uint32_t*)(m_backbuffer+(y*m_current->bytes_per_line + x*4));
		*pos = color;
	} else {
		unsigned char b = color & 0xff;
		unsigned char g = (color>>8) & 0xff;
		unsigned char r = (color>>16) & 0xff;
		unsigned char a = (color>>24) & 0xff;
		putPixel(x, y, r,g,b,a);
	}
}

void FB::clear()
{
	m.lock();
	if (m_backbuffer!=NULL) Mem::set(m_backbuffer, 0, m_size);
	if (m_buffer!=NULL) Mem::set(m_buffer, 0, m_size);
	m.unlock();
	//swap();
#if 0
	unsigned char *dest = backbuffer+(current->height*current->bytes_per_line);
	unsigned char *ptr = backbuffer;
	while (ptr<dest) {
		*ptr++ = 0;
	}
#endif
}
