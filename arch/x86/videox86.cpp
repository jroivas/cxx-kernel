#include "videox86.h"
#include "port.h"

//#define VIDEO_MEMORY_LOCATION 0xC00B8000
#define VIDEO_MEMORY_LOCATION 0xB8000
#define PORT 0x3D4

VideoX86::VideoX86() : Video()
{
	m_videomem = (unsigned short *)VIDEO_MEMORY_LOCATION;
}

void VideoX86::setCursor()
{
	unsigned int pos = m_y*width() + m_x;

	Port::out(PORT, 0x0F);
	Port::out(PORT+1, pos&0xFF);
	Port::out(PORT, 0x0E);
	Port::out(PORT+1, (pos>>8)&0xFF);
}
