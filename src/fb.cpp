#include "fb.h"
#include "string.hh"
#include "types.h"
#include "mm.h"
#include "arch/platform.h"
#include "memcopy.h"

static Mutex __fb_mutex;

FB::FB()
    :
    m_buffer(nullptr),
    m_backbuffer(nullptr),
    m_double_buffer(nullptr),
    m_current(nullptr),
    m_direct(false),
    m_configured(false)
{
    m = &__fb_mutex;
}

FB::~FB()
{
    freeBuffers();
}

void FB::setSingleBuffer()
{
    MutexLocker lock(&__fb_mutex);

    m_double_buffer = false;
    if (m_buffer != nullptr) {
        MM::instance()->free(m_buffer);
    }
    m_buffer = nullptr;
}

void FB::setDirect()
{
    MutexLocker lock(&__fb_mutex);

    m_direct = true;
    if (m_backbuffer != nullptr) {
        MM::instance()->free(m_backbuffer);
    }
    if (m_buffer!=nullptr) {
        MM::instance()->free(m_buffer);
    }
    m_backbuffer = m_current->base;
    m_buffer = m_current->base;
}

void FB::allocBuffers()
{
    if (m_direct) return;
    MutexLocker lock(&__fb_mutex);

    m_size = m_current->bytes_per_line*(m_current->height);
    m_backbuffer = (unsigned char*)MM::instance()->alloc(m_size, MM::AllocClear);
    if (m_backbuffer!=nullptr) {
        m_backbuffer[0] = 0xfe;
        m_backbuffer[m_size/2] = 0xed;
        m_backbuffer[m_size-1] = 0xdc;
        if (m_backbuffer==nullptr
            || m_backbuffer[0] != 0xfe
            || m_backbuffer[m_size/2] != 0xed
            || m_backbuffer[m_size-1] != 0xdc) {
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
            if (m_buffer==nullptr || m_buffer[0] != 0xef || m_buffer[m_size/2] != 0xde || m_buffer[m_size-1] != 0xec) {
                m_double_buffer = false;
                MM::instance()->free(m_buffer);
                m_buffer = nullptr;
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
        if (buffer!=nullptr) buffer[i] = 0;
        if (backbuffer!=nullptr) backbuffer[i] = 0;
    }
#endif
    //Platform::video()->printf("Allocbuffers: %x %x %x sizes: %d\n",m_current->base,m_buffer,m_backbuffer,m_current->bytes_per_line*(m_current->height));
}

void FB::freeBuffers()
{
    MutexLocker lock(&__fb_mutex);
    if (m_buffer!=nullptr) {
        MM::instance()->free(m_buffer);
        m_buffer = nullptr;
    }
    if (m_backbuffer!=nullptr) {
        MM::instance()->free(m_backbuffer);
        m_backbuffer = nullptr;
    }
}

bool FB::configure(ModeConfig *mode)
{
    if (mode == nullptr) return false;

    m_current = mode;

    allocBuffers();

    m_configured = true;
    return true;
}

unsigned char *FB::data()
{
    if (m_buffer == nullptr) return nullptr;
    return m_buffer;
}

void FB::swap()
{
    if (m_buffer == nullptr) return;
    if (m_backbuffer == nullptr) return;
    if (m_direct) return;
    if (!m_double_buffer) {
        m_buffer = m_backbuffer;
        return;
    }

    MutexLocker lock(&__fb_mutex);
    if (m_buffer!=nullptr && !m_direct) {
        memcpy_opt(m_buffer, m_backbuffer, m_size);
        //Mem::copy(m_buffer, m_backbuffer, m_size);
    }
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
    if (m_backbuffer==nullptr) return;
    if (m_current==nullptr) return;

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
    if (m_backbuffer!=nullptr && m_current!=nullptr && m_current->depth==32) {
        x %= m_current->width;
        y %= m_current->height;
        uint32_t *pos = (uint32_t*)(m_backbuffer+(y*m_current->bytes_per_line + x*4));
        *pos = color;
    } else {
        unsigned char b = color & 0xff;
        unsigned char g = (color>>8) & 0xff;
        unsigned char r = (color>>16) & 0xff;
        unsigned char a = (color>>24) & 0xff;
        putPixel(x, y, r, g, b, a);
    }
}

void FB::clear()
{
    MutexLocker lock(&__fb_mutex);
    if (m_backbuffer != nullptr) Mem::set(m_backbuffer, 0, m_size);
    if (m_buffer != nullptr) Mem::set(m_buffer, 0, m_size);
    //swap();
#if 0
    unsigned char *dest = backbuffer+(current->height*current->bytes_per_line);
    unsigned char *ptr = backbuffer;
    while (ptr<dest) {
            *ptr++ = 0;
    }
#endif
}
