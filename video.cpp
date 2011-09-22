#include "video.h"

#define VIDEO_MEMORY_LOCATION 0xB8000
#define VIDEO_COLOR_MASK 0x0700
#define CHAR_LF 0x0A
#define CHAR_CR 0x0D
#define EMPTY ' '

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
		m_videomem[i++] = EMPTY | VIDEO_COLOR_MASK;
	}

	m_x = 0;
	m_y = 0;
}

void Video::print(const char *cp)
{
	const char *str = cp;
	const char *ch;

	for (ch = str; *ch; ch++) {
		if (*ch==CHAR_CR) {
			m_x = 0;
		}
		else if (*ch==CHAR_LF) {
			m_x = 0;
			m_y++;
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
}
