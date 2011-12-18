#ifndef FONT_H
#define FONT_H

#include "fb.h"
#include "types.h"

class Font
{
public:
	Font();
	void drawFont(FB *fb, int x, int y, unsigned char c);

protected:
	uint32_t font_color;
};

#endif
