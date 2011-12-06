#include "fb.h"
#include "string.h"
#include "types.h"
#include "mm.h"
#include "arch/platform.h"

FB::FB()
{
	current = NULL;
	buffer = NULL;
	backbuffer = NULL;
	double_buffer = true;
	direct = false;
}

FB::~FB()
{
	freeBuffers();
}

void FB::setSingleBuffer()
{
	double_buffer = false;
}

void FB::allocBuffers()
{
	size = current->bytes_per_line*(current->height);
	backbuffer = (unsigned char*)MM::instance()->alloc(size);
	if (backbuffer!=NULL) {
		backbuffer[0] = 0xfe;
		if (backbuffer[0] != 0xfe) {
			direct = true;
			backbuffer = current->base;
			MM::instance()->free(backbuffer);
		} else {
			buffer = (unsigned char*)MM::instance()->alloc(size);
		}
	}
	//Platform::video()->printf("Allocbuffers: %x %x sizes: %d\n",buffer,backbuffer,current->bytes_per_line*(current->height));
}

void FB::freeBuffers()
{
	if (buffer!=NULL) {
		MM::instance()->free(buffer);
		buffer = NULL;
	}
	if (backbuffer!=NULL) {
		MM::instance()->free(backbuffer);
		backbuffer = NULL;
	}
}

bool FB::configure(ModeConfig *mode)
{
	if (mode==NULL) return false;

	current = mode;

	allocBuffers();

	return true;
}

unsigned char *FB::data()
{
	if (backbuffer == NULL) return NULL;
	return backbuffer;
}

void FB::swap()
{
	if (buffer == NULL) return;
	if (backbuffer == NULL) return;
	if (!double_buffer) {
		buffer = backbuffer;
		return;
	}

	if (!direct) {
		Mem::copy(buffer, backbuffer, size);
	}
}

void FB::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (backbuffer==NULL) return;
	if (current==NULL) return;

	x %= current->width;
	y %= current->height;

	switch (current->depth) {
		case 16:
			{
			unsigned short *pos = (unsigned short*)(backbuffer+(y*current->bytes_per_line + x*2));
			unsigned short color = 0;
			color += (r & 0x7C) << 8;
			color += (g & 0x7E) << 2;
			color += (b & 0x7C) >> 3;
			*pos = color;
			}
			break;
		/*case 24:
			{
			unsigned char *pos = (unsigned char*)(backbuffer+(y*current->bytes_per_line + x*4));
			*pos++ = r;
			*pos++ = g;
			*pos++ = b;
			}
			break;*/
		case 24:
			{
			unsigned char *pos = (unsigned char*)(backbuffer+(y*current->bytes_per_line + x*3));
			*pos++ = r;
			*pos++ = g;
			*pos = b;
			}
			break;
		case 32:
			{
			unsigned int *pos = (unsigned int*)(backbuffer+(y*current->bytes_per_line + x*4));
			unsigned int color = (a<<24)+(r<<16)+(g<<8)+b;
			*pos = color;
			}
			break;
		default:
			// Unsupported
			break;
	}
}

void FB::putPixel(int x, int y, unsigned int color)
{
	if (backbuffer!=NULL && current!=NULL && current->depth==32) {
		x %= current->width;
		y %= current->height;
		unsigned int *pos = (unsigned int*)(backbuffer+(y*current->bytes_per_line + x*4));
		*pos = color;
	} else {
		unsigned char b = color & 0xff;
		unsigned char g = (color>>8) & 0xff;
		unsigned char r = (color>>16) & 0xff;
		unsigned char a = (color>>24) & 0xff;
		putPixel(x, y, r,g,b,a);
	}
}

void FB::clear()
{
	if (backbuffer==NULL) return;
#if 0
	unsigned char *dest = backbuffer+(current->height*current->bytes_per_line);
	unsigned char *ptr = backbuffer;
	while (ptr<dest) {
		*ptr++ = 0;
	}
#endif
}
