#include "font.h"
#include "3rdparty/font/gfb.h"

extern const struct gfb_font bold8x16;

Font::Font()
{
	font_color = 0xFFFFFF;
}

void Font::drawFont(FB *fb, int x, int y, unsigned char c)
{
	if (fb==NULL) return;

	for (int i=0; i<bold8x16.height; i++) {
		unsigned char ch = bold8x16.data[(unsigned int)c*bold8x16.height+i];
		for (int j=0; j<bold8x16.width; j++) {
			if (ch & (1<<j)) {
				//fb->putPixel(x+j,y+i,font_color);
				//fb->putPixel(x+(bold8x16.width-j),y+i,255,255,255);
				fb->putPixel(x+(bold8x16.width-j),y+i,font_color);
			}
		}
	}
}
