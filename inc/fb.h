#ifndef FB_H
#define FB_H

#include "mutex.hh"

class FB
{
public:
    struct ModeConfig {
        int width:32;
        int height:32;
        unsigned short bytes_per_line:16;
        unsigned char depth:8;
        unsigned char unused:8;
        unsigned char rsize;
        unsigned char gsize;
        unsigned char bsize;
        unsigned char asize;
        unsigned char *base;
        unsigned int id;
    };

    FB();
    virtual ~FB();
    virtual ModeConfig *query(ModeConfig *prefer) = 0;
    virtual bool configure(ModeConfig *mode);
    virtual void blit() = 0;
    void swap();
    virtual void clear();
    unsigned char *data();
    void setSingleBuffer();
    void setDirect();
    //void putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
    void putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a=0);
    void putPixel(int x, int y, unsigned int color);
    bool isConfigured() { return m_configured; }
    virtual bool isValid() {
        return true;
    }

protected:
    void allocBuffers();
    void freeBuffers();

    unsigned char *m_buffer;
    unsigned char *m_backbuffer;
    bool m_double_buffer;
    ModeConfig *m_current;
    unsigned int m_size;
    bool m_direct;
    bool m_configured;
    Mutex m;
};
#endif
