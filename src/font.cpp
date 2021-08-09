#include "font.h"

#define RAWFONT(X)(X)

KernelFont::KernelFont()
{
    font_color = 0xFFFFFF;
    current = fontGetDefault();
}

void KernelFont::drawFontFill(FB *fb, int x, int y, unsigned char c, int fill)
{
    if (fb == nullptr) return;
    if (current == nullptr) return;

#ifdef FEATURE_GRAPHICS
    for (int32_t i=0; i < RAWFONT(current)->height; i++) {
        int32_t p = RAWFONT(current)->width;
        uint32_t index = (uint32_t)(c * RAWFONT(current)->height + i);
        unsigned char ch = RAWFONT(current)->data[index];
        for (int32_t j=0; j < RAWFONT(current)->width; j++) {
            if (ch & (1 << (--p))) {
                fb->putPixel(x+j, y+i, font_color);
            } else if (fill)
                fb->putPixel(x+j, y+i, 0);
        }
    }
#else
    (void)x;
    (void)y;
    (void)c;
    (void)fill;
#endif
}

void KernelFont::drawFont(FB *fb, int x, int y, unsigned char c)
{
    drawFontFill(fb, x, y, c, 0);
}

uint8_t KernelFont::width()
{
    if (current == nullptr) return 0;
    return RAWFONT(current)->width;
}

uint8_t KernelFont::height()
{
    if (current == nullptr) return 0;
    return RAWFONT(current)->height;
}
