#include "fb.h"
#ifdef __linux__
#include <stdlib.h>
#else
#include "types.h"
#include "mm.h"
#endif

FB::FB()
{
	current = NULL;
	buffer = NULL;
	backbuffer = NULL;
	double_buffer = true;
	tmp_w = 0;
	tmp_h = 0;
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
	buffer = (unsigned char*)malloc(current->bytes_per_line*(current->height+1));
	backbuffer = (unsigned char*)malloc(current->bytes_per_line*(current->height+1));
}

void FB::freeBuffers()
{
	if (buffer!=NULL) {
		free(buffer);
		buffer = NULL;
	}
	if (backbuffer!=NULL) {
		free(backbuffer);
		backbuffer = NULL;
	}
}

bool FB::configure(ModeConfig *mode)
{
	if (mode==NULL) return false;

	current = mode;
	tmp_w = mode->width-1;
	tmp_h = mode->height-1;

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

	unsigned char *tmp = buffer;
	buffer = backbuffer;
	backbuffer = tmp;
}

void FB::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{

	if (backbuffer==NULL) return;
	if (current==NULL) return;
#if 0
	if (x>tmp_w) x %= current->width;
	if (y>tmp_h) y %= current->height;
#else
	x %= current->width;
	y %= current->height;
#endif
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
		case 32:
			{
			unsigned int *pos = (unsigned int*)(backbuffer+(y*current->bytes_per_line + x*4));
			unsigned int color = (a<<24)+(r<<16)+(g<<8)+b;
			/*
			unsigned int color = 0;
			color += a;
			color <<= 8;
			color += r;
			color <<= 8;
			color += g;
			color <<= 8;
			color += b;
			*/
			*pos = color;
			/*
			unsigned char *pos = (unsigned char*)(backbuffer+(y*current->bytes_per_line + x*4));
			*pos++ = a;
			*pos++ = r;
			*pos++ = g;
			*pos++ = b;
			*/
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
	unsigned char *dest = backbuffer+(current->height*current->bytes_per_line);
	unsigned char *ptr = backbuffer;
	while (ptr<dest) {
		*ptr++ = 0;
	}
}
