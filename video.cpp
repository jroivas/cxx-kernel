#include "video.h"
#include "string.h"
#include "port.h"

#define VIDEO_MEMORY_LOCATION 0xB8000
//#define VIDEO_MEMORY_LOCATION 0xC00B8000
#define VIDEO_COLOR_MASK 0x0700
#define CHAR_LF 0x0A
#define CHAR_CR 0x0D
#define CHAR_EMPTY ' '
#define TAB_SIZE 8
#define SCROLL_SIZE 1
#define PORT 0x3D4

Video::Video()
{
	m_x = 0;
	m_y = 0;

	m_width = 80;
	m_height = 25;

	m_videomem = (unsigned short *)VIDEO_MEMORY_LOCATION;
}

Video::~Video()
{
	m_x = 0;
	m_y = 0;
}

unsigned int Video::size()
{
	return width()*height();
}

unsigned int Video::width()
{
	return m_width;
}

unsigned int Video::height()
{
	return m_height;
}

void Video::resize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void Video::clear()
{
	unsigned int i = 0;

	while (i<size()) {
		m_videomem[i++] = CHAR_EMPTY | VIDEO_COLOR_MASK;
	}

	m_x = 0;
	m_y = 0;
}

void Video::setCursor()
{
	unsigned int pos = m_y*width() + m_x;

	Port::out(PORT, 0x0F);
	Port::out(PORT+1, pos&0xFF);
	Port::out(PORT, 0x0E);
	Port::out(PORT+1, (pos>>8)&0xFF);
}

void Video::scroll()
{
	if (m_y>=height()) {
		unsigned int size = (height()-1)*width();
		Mem::copy(m_videomem, m_videomem + SCROLL_SIZE*width(), size);
		Mem::setw(m_videomem + size, ' '|VIDEO_COLOR_MASK, width());
		m_y = height()-1;
	}
}

void Video::print(const char *cp)
{
	const char *str = cp;
	const char *ch;

	for (ch = str; *ch; ch++) {
		if (*ch=='\r') {
			m_x = 0;
		}
		else if (*ch=='\n') {
			m_x = 0;
			m_y++;
		}
		else if (*ch=='\b') {
			if (m_x==0 && m_y>0) {
				m_y--;
				m_x = width()-1;
			} else if (m_y==0 && m_x==0) {
				//Do nothing
			} else {
				m_x--;
			}
		}
		else if (*ch=='\t') {
			for (int i=0; i<TAB_SIZE; i++) putCh(' ');
		} else {
			putCh(*ch);
		}
	}
}

void Video::putCh(char c)
{
	if (m_x>=width()) {
		m_x = 0;
		m_y++;
	}

	unsigned int offset = m_y*width() + m_x; 
	if (offset>=size()) {
		clear(); //TODO scrolling
	}

	m_videomem[offset] = c | VIDEO_COLOR_MASK;
	m_x++;

	scroll();
	setCursor();
}
