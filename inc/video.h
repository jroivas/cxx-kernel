#ifndef VIDEO_H
#define VIDEO_H

#include "font.h"
#include <stdarg.h>

class Video
{
public:
    Video();
    virtual ~Video();

    static Video *get();
    void clear();
    int printf(const char *fmt, ...);
    int vprintf(const char *fmt, va_list al);
    int print(const char *cp);
    virtual void putCh(char c);
    virtual void putChDebug(char c);
    void handleChar(char c);

    void resize(int width, int height);
    void adapt(int width, int height);
    void scroll();

    unsigned int size() const;
    unsigned int width() const;
    unsigned int height() const;

protected:
    int print_ull(unsigned long long val, int radix=10, int fmtcnt=0);
    int print_l(long val, int radix=10, int fmtcnt=0);
    int print_prenull(int cnt, int fmtcnt);

    virtual void setCursor() = 0;
    unsigned short *m_videomem;
    unsigned int m_x;
    unsigned int m_y;
    unsigned int m_width;
    unsigned int m_height;
    KernelFont *m_font;
};

#endif
