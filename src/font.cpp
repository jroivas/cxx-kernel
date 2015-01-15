#include "font.h"

#define RAWFONT(X)(X)

KernelFont::KernelFont()
{
    font_color = 0xFFFFFF;
    current = fontGetDefault();
}

void KernelFont::drawFont(FB *fb, int x, int y, unsigned char c)
{
    if (fb == NULL) return;
    if (current == NULL) return;

#if 1
    for (int32_t i=0; i<RAWFONT(current)->height; i++) {
        int32_t p = RAWFONT(current)->width;
        unsigned char ch = RAWFONT(current)->data[(unsigned int)c*RAWFONT(current)->height+i];
        for (int32_t j=0; j<RAWFONT(current)->width; j++) {
            if (ch & (1<<(--p))) {
                fb->putPixel(x+j,y+i,font_color);
            }
        }
    }
#else
    for (int32_t i=0; i<RAWFONT(current).height; i++) {
        int32_t p = RAWFONT(current).width;
        unsigned char ch = RAWFONT(current).data[(unsigned int)c*RAWFONT(current).height+i];
        for (int32_t j=0; j<RAWFONT(current).width; j++) {
            if (ch & (1<<(--p))) {
                fb->putPixel(x+j,y+i,font_color);
            }
        }
    }
#endif
}

uint8_t KernelFont::width()
{
    if (current==NULL) return 0;
    return RAWFONT(current)->width;
}

uint8_t KernelFont::height()
{
    if (current==NULL) return 0;
    return RAWFONT(current)->height;
}
