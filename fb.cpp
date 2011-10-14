#include "fb.h"
#include "types.h"

FB::FB()
{
	current = NULL;
	buffer = NULL;
	backbuffer = NULL;
}

unsigned char *FB::data()
{
	if (buffer == NULL) return NULL;
	return buffer;
}

void FB::swap()
{
	if (buffer == NULL) return;
	if (backbuffer == NULL) return;

	unsigned char *tmp = buffer;
	buffer = backbuffer;
	backbuffer = tmp;

	//blit(); //TODO/XXX do we want this here?
}

void FB::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (backbuffer==NULL) return;
	if (current==NULL) return;
	switch (current->depth) {
		case 16:
			{
			unsigned short *pos = (unsigned short*)(y*current->bytes_per_line + x*2);
			unsigned short color = 0;
			color += (r & 0x7C) << 8;
			color += (g & 0x7E) << 2;
			color += (b & 0x7C) >> 3;
			*pos = color;
			}
			break;
		case 24:
			{
			unsigned char *pos = (unsigned char*)(y*current->bytes_per_line + x*3);
			*pos++ = r;
			*pos++ = g;
			*pos++ = b;
			}
			break;
		case 32:
			{
			unsigned int *pos = (unsigned int*)(y*current->bytes_per_line + x*4);
			/*unsigned int color = 0;
			color += a;
			color <<= 8;
			color += r;
			color <<= 8;
			color += g;
			color <<= 8;
			color += b;
			*pos = color;
			*/
			*pos++ = a;
			*pos++ = r;
			*pos++ = g;
			*pos++ = b;
			}
			break;
		default:
			// Unsupported
			break;
	}
}

void FB::putPixel(int x, int y, unsigned int color)
{
	unsigned char b = color & 0xff;
	unsigned char g = (color>>8) & 0xff;
	unsigned char r = (color>>16) & 0xff;
	unsigned char a = (color>>24) & 0xff;
	putPixel(x, y, r,g,b,a);
}
