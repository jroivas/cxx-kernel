#ifndef FONT_H
#define FONT_H

#include "fb.h"
#include "types.h"
#include "3rdparty/font/raw_font.h"

class KernelFont
{
public:
	KernelFont();
	void drawFont(FB *fb, int x, int y, unsigned char c);
	uint8_t width();
	uint8_t height();

protected:
	uint32_t font_color;
	struct raw_font *current;
};

#endif
