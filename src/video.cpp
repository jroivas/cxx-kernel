#include "video.h"
#include "string.hh"
#include "mm.h"
#include "font.h"
#include "arch/platform.h"
#include <stdarg.h>

#define VIDEO_COLOR_MASK 0x0700
#define CHAR_LF 0x0A
#define CHAR_CR 0x0D
#define CHAR_EMPTY ' '
#define TAB_SIZE 8
#define SCROLL_SIZE 1

static Video *__global_video = nullptr;

Video *Video::get()
{
    if (__global_video==nullptr) {
        __global_video = Platform::video();
    }
    return __global_video;
}

Video::Video()
{
    m_x = 0;
    m_y = 0;

    m_width = 80;
    m_height = 25;

    m_videomem = nullptr;
    m_font = nullptr;
}

Video::~Video()
{
    m_x = 0;
    m_y = 0;
}

unsigned int Video::size() const
{
    return width() * height();
}

unsigned int Video::width() const
{
    return m_width;
}

unsigned int Video::height() const
{
    return m_height;
}

void Video::resize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void Video::adapt(int width, int height)
{
    if (m_font == nullptr) {
        m_font = new KernelFont();
        m_x = 0;
        m_y = 0;
    }
    resize(width / m_font->width(), height / m_font->height());
}

void Video::clear()
{
    if (Platform::fb() != nullptr && Platform::fb()->isConfigured()) {
        Platform::fb()->clear();
    } else {
        if (m_videomem == nullptr) return;
        unsigned int i = 0;
        unsigned int csize = size();

        while (i < csize) {
            m_videomem[i++] = CHAR_EMPTY | VIDEO_COLOR_MASK;
        }
    }
    m_x = 0;
    m_y = 0;
}

void Video::scroll()
{
    if (Platform::fb() != nullptr && Platform::fb()->isConfigured()) {
        if (m_y >= height()) {

            uint32_t scroll_size = SCROLL_SIZE * m_font->height() * Platform::fb()->bpl();

            Mem::copy(Platform::fb()->data(), Platform::fb()->data() + scroll_size, Platform::fb()->size() - scroll_size);
            Mem::set(Platform::fb()->data() + Platform::fb()->size() - scroll_size, 0, scroll_size);
            m_y -= SCROLL_SIZE;
        }
    } else {
        if (m_videomem == nullptr) return;
        if (m_y >= height()) {
            unsigned int ss = (height() - 2) * width();
            Mem::copy(m_videomem, m_videomem + SCROLL_SIZE * width() * 2, ss * 2);
            Mem::setw(m_videomem + ss, 0, width());
            //Mem::setw(m_videomem + ss-width(), ' '|VIDEO_COLOR_MASK, width());
            m_y = height() - 2;
        }
    }
    if (m_x >= width()) m_x = 0;
    if (m_y > height()) m_y = height() - 2;
}

int Video::print(const char *cp)
{
    const char *str = cp;
    const char *ch;
    int print_cnt = 0;

    for (ch = str; *ch; ch++) {
        putCh(*ch);
        ++print_cnt;
    }
    return print_cnt;
}

int Video::print_prenull(int cnt, int fmtcnt)
{
    if (cnt < fmtcnt) {
        char s2[256];
        int cc = fmtcnt - cnt;
        if (cc > 255) {
            cc=255;
        }
        for (int i = 0; i < cc; i++) {
            s2[i] = '0';
        }
        s2[cc] = 0;
        return print(s2);
    }
    return 0;
}

int Video::print_l(long val, int radix, int fmtcnt)
{
    int print_cnt = 0;
    if (val == 0) {
        print_cnt += print_prenull(1, fmtcnt);
        putCh('0');
        ++print_cnt;
        return print_cnt;
    }
    bool n = false;
    if (val < 0) {
        val *= -1;
        n = true;
    }

    int l = 0;
    int tmp = val;
    while (tmp > 0) {
        tmp /= radix;
        l++;
    }

    if (n) l++;
    if (l == 0) {
        l = 1;
    }

    int cnt = 0;
    char s[256];
    s[l] = 0;
    l--;

    while (l >= 0) {
        char t = val % radix;
        if (t < 10) {
            s[l] = '0' + t;
        } else {
            s[l] = 'A' + (t - 10);
        }
        val /= radix;
        l--;
        cnt++;
    }
    if (n) {
        s[0] = '-';
    }
    print_cnt += print_prenull(cnt, fmtcnt);
    print_cnt += print(s);
    return print_cnt;
}

int Video::print_ull(unsigned long long val, int radix, int fmtcnt)
{
    int print_cnt = 0;
    if (val == 0) {
        print_cnt += print_prenull(1, fmtcnt);
        putCh('0');
        ++print_cnt;
        return print_cnt;
    }
    int l = 0;
    unsigned long long tmp = val;
    while (tmp > 0) {
        tmp /= radix;
        l++;
    }
    if (l == 0) {
        l = 1;
    }

    int cnt = 0;
    char s[256];
    s[l] = 0;
    s[l+1] = 0;
    l--;
    while (l >= 0) {
        char t = val % radix;
        if (t < 10) {
            s[l] = '0' + t;
        } else {
            s[l] = 'A' + (t - 10);
        }
        val /= radix;
        l--;
        cnt++;
    }
    print_cnt += print_prenull(cnt, fmtcnt);
    print_cnt += print(s);

    return print_cnt;
}

int Video::vprintf(const char *fmt, va_list al)
{
    const char *str = fmt;
    const char *ch;
    bool format_char = false;
    bool s_signed = true;
    int l_cnt = 0;
    int fmtcnt = 0;
    int print_count = 0;

    for (ch = str; *ch; ch++) {
        if (format_char) {
            if (*ch == '%') {
                putCh('%');
                ++print_count;
                format_char = false;
            }
            else if (*ch == 's') {
                print_count += print(va_arg(al, const char*));
                format_char = false;
            }
            else if (*ch == 'l') {
                //s_signed = false;
                l_cnt++;
            }
            else if (*ch == 'x') {
                if (l_cnt == 0) {
                    print_count += print_ull(va_arg(al, unsigned int), 16, fmtcnt);
                } else if (l_cnt == 1) {
                    print_count += print_ull(va_arg(al, unsigned long), 16, fmtcnt);
                } else if (l_cnt == 2) {
                    print_count += print_ull(va_arg(al, unsigned long long), 16, fmtcnt);
                }
                format_char = false;
            }
            else if (*ch == 'c') {
                putCh(va_arg(al, unsigned int));
                ++print_count;
                format_char = false;
            }
            else if (*ch == 'd') {
                if (!s_signed) {
                    print_count += print_ull(va_arg(al, unsigned int), 10, fmtcnt);
                } else {
                    print_count += print_l(va_arg(al, long int), 10, fmtcnt);
                }
                format_char = false;
            } else if (*ch == 'u') {
                if (l_cnt == 0) {
                    print_count += print_ull(va_arg(al, unsigned int), 10, fmtcnt);
                } else if (l_cnt == 1) {
                    print_count += print_ull(va_arg(al, unsigned long), 10, fmtcnt);
                } else if (l_cnt == 2) {
                    print_count += print_ull(va_arg(al, unsigned long long), 10, fmtcnt);
                }
#if 0
                s_signed = false;
                if (!s_signed) {
                    print_ull(va_arg(al, unsigned long));
                } else {
                    print_ull(va_arg(al, long));
                }
#endif
                format_char = false;
            }
            else if (*ch >= '0' && *ch <= '9') {
                fmtcnt *= 10;
                fmtcnt += (*ch-'0');
            }
            else {
                //FIXME
            }
        }
        else if (*ch == '%') {
            fmtcnt = 0;
            format_char = true;
            s_signed = true;
            l_cnt = 0;
        }
        else if (*ch == '\r') {
            m_x = 0;
        }
        else if (*ch == '\n') {
            m_x = 0;
            m_y++;
            putCh(*ch);
            ++print_count;
        }
        else if (*ch == '\b') {
            if (m_x == 0 && m_y > 0) {
                m_y--;
                m_x = width() - 1;
            } else if (m_y == 0 && m_x == 0) {
                //Do nothing
            } else {
                m_x--;
            }
        }
        else if (*ch == '\t') {
            m_x += TAB_SIZE;
        } else {
            putCh(*ch);
            ++print_count;
        }
    }
    return print_count;
}

int Video::printf(const char *fmt, ...)
{
    va_list al;
    va_start(al, fmt);

    int res = vprintf(fmt, al);

    va_end(al);

    return res;
}

void Video::handleChar(char c)
{
    if (c == 0) {
        return;
    }
    if (c == '\n') {
        m_x = 0;
        m_y++;
        putCh(c);
        return;
    }
    putCh(c);
}

void Video::putCh(char c)
{
    if (c == '\n') {
        scroll();
        return;
    }
    if (c == '\b') {
        // Back one char if possible
        if (m_x > 0) {
            m_x--;
        }
        // Draw empty at current char
        m_font->drawFontFill(
            Platform::fb(),
            m_x * m_font->width(),
            m_y * m_font->height(),
            ' ', 1);
        return;
    } else if (m_x >= width()) {
            m_x -= width();
            m_y++;
    }

    scroll();

    if (Platform::fb() != nullptr && Platform::fb()->isConfigured()) {
        if (m_font == nullptr) {
            m_font = new KernelFont();
            //clear();
            m_x = 0;
            m_y = 0;
        }
        m_font->drawFont(
            Platform::fb(),
            m_x * m_font->width(),
            m_y * (m_font->height() + 0),
            c);
        m_x++;
    } else {
        if (m_videomem == nullptr) return;
        unsigned int offset = m_y * width() + m_x;

        m_videomem[offset] = c | VIDEO_COLOR_MASK;

        m_x++;
        setCursor();
    }
}
